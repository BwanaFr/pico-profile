#include <stdio.h>
#include "pico/stdlib.h"

#include "pico-profile.pio.h"
#include "io_functions.h"
#include "protocol.h"

#include "ff.h"
#include "sd_card.h"

//PIO for command state machine
#define CMD_PIO pio1
#define CMD_SM 0
uint pio_cmd_offs = 0;
pio_sm_config pio_cmd_cfg = {0,0,0,0};

//FATFS for SD card data exchange
FATFS fs;

/**
 * Mount the SD card
 */
static FRESULT run_mount() {
    char *arg1 = "0:";
    FATFS *p_fs = &fs;
    return f_mount(p_fs, arg1, 1);
}


// static void run_unmount() {
//     char *arg1 = "0:";
//     FRESULT fr = f_unmount(arg1);
//     if (FR_OK != fr) printf("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
// }

// static void run_mount() {
//     char *arg1 = "0:";
//     FATFS *p_fs = &fs;
//     FRESULT fr = f_mount(p_fs, arg1, 1);
//     if (FR_OK != fr) printf("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
// }

// static void run_getfree() {
//     char *arg1 = "0:";
//     DWORD fre_clust, fre_sect, tot_sect;
//     /* Get volume information and free clusters of drive */
//     FATFS *p_fs = &fs;
//     FRESULT fr = f_getfree(arg1, &fre_clust, &p_fs);
//     if (FR_OK != fr) {
//         printf("f_getfree error: %s (%d)\n", FRESULT_str(fr), fr);
//         return;
//     }
//     /* Get total sectors and free sectors */
//     tot_sect = (p_fs->n_fatent - 2) * p_fs->csize;
//     fre_sect = fre_clust * p_fs->csize;
//     /* Print the free space (assuming 512 bytes/sector) */
//     printf("%10lu KiB total drive space.\n%10lu KiB available.\n", tot_sect / 2,
//            fre_sect / 2);
// }

// static void run_test_write(){
//     FIL f;
//     FRESULT fr;     /* FatFs return code */
//     char line[10];
//     UINT written;

//     fr = f_open(&f, "message.txt", FA_READ);
//     if(fr){
//         return;
//     }
//     for(int i=0;i<20;++i){
//         sprintf(line, "Line %d\n", (i+1));
//         // f_write(f, strlen(line), )
//     }
// }

// void putToBus(uint8_t data){     
//     gpio_put(OD0, (data & 0x1) == 1);
//     gpio_put(OD1, (((data>>1) & 0x1) == 1));
//     gpio_put(OD2, (((data>>2) & 0x1) == 1));
//     gpio_put(OD3, (((data>>3) & 0x1) == 1));
//     gpio_put(OD4, (((data>>4) & 0x1) == 1));
//     gpio_put(OD5, (((data>>5) & 0x1) == 1));
//     gpio_put(OD6, (((data>>6) & 0x1) == 1));
//     gpio_put(OD7, (((data>>7) & 0x1) == 1));
// }

// uint8_t getBus(){
//     uint8_t ret = 0;
//     uint32_t input = gpio_get_all();
//     ret = (input >> 16) & 0x7F;
//     if(((input >> 26) & 0x1) != 0x0){
//         ret |= 0x80;
//     }
//     return ret;
// }

/**
 * Utility to print command details to serial
 */
void printCommand(uint8_t *cmd){
    uint32_t block = cmd[0] | (cmd[1] << 8) | (cmd[2] << 16);
    switch (cmd[0])
    {
    case READ_CMD:
        printf("Read block $%lx, retries:%u, spare threshold:%u\n", block, cmd[4], cmd[5]);
        break;
    case WRITE_CMD:
        printf("Write block $%lx\n", block);
        break;
    case WRITE_VERIFY_CMD:
        printf("Write & verify block $%lx, retries:%u, spare threshold:%u\n", block, cmd[4], cmd[5]);
        break;
    case WRITE_SPARE:
        printf("Write & Force Sparing block $%lx\n", block);
        break;
    default:
        printf("Unsupported command\n");
        break;
    }
    
}

/**
 * Convert GPIO from state machines to data byte
 */
uint8_t gpioToByte(uint32_t value) {
    return (value & 0x7F) | ((value >> 3) & 0x80);
}

/**
 * Handle commands sent by the Apple
 */
void handleCommand() {
    uint8_t cmd[6];
    printf("Waiting for command\n");
    uint8_t q = gpioToByte(pio_sm_get_blocking(CMD_PIO, CMD_SM));
    if(q != 0x55){
        //Error, Apple must reply with 0x55
        printf("ERROR : Bad acknowledge read");
        pio_sm_restart(CMD_PIO, CMD_SM);
        return;
    }
    //Accept command
    pio_sm_put_blocking(CMD_PIO, CMD_SM, 0x1);
    for(int i=0;i<6;++i){
        cmd[i] = gpioToByte(pio_sm_get_blocking(CMD_PIO, CMD_SM));
    }
    //Accept command, answer 2
    pio_sm_put_blocking(CMD_PIO, CMD_SM, 0x2);
    //Gets the 0x55 ack
    q = gpioToByte(pio_sm_get_blocking(CMD_PIO, CMD_SM));
    if(q != 0x55){
        //Error, Apple must reply with 0x55
        printf("ERROR : Bad acknowledge read");
        pio_sm_restart(CMD_PIO, CMD_SM);
        return;
    }
    if(cmd[0] == 0x0){
        //Reset the state machine
        pio_sm_put_blocking(CMD_PIO, CMD_SM, 0x0);
    }else{
        //Continue with the state machine
        pio_sm_put_blocking(CMD_PIO, CMD_SM, 0x01);
        q = gpioToByte(pio_sm_get_blocking(CMD_PIO, CMD_SM));
        pio_sm_put_blocking(CMD_PIO, CMD_SM, 0x0);
    }
    printCommand(cmd);
}

static void isr() {
    printf(".\n");
    pio_interrupt_clear(CMD_PIO, 0);
}

int main() {
    stdio_init_all();
    initializeGPIO();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    printf("Pico-profile\n");
    //Initialize the command state machine
    //pio_cmd_offs = pio_add_program(CMD_PIO, &pico_profile_cmd_program);
    //pio_cmd_cfg = pico_profile_cmd_pio_init(CMD_PIO, CMD_SM, pio_cmd_offs);
    //Cmd handshake
    pio_cmd_offs = pio_add_program(CMD_PIO, &pico_profile_cmd_hchk_program);
    pio_cmd_cfg = pico_profile_cmd_hchk_pio_init(CMD_PIO, CMD_SM, pio_cmd_offs);
    /*irq_set_exclusive_handler(PIO1_IRQ_0, isr);
    irq_set_enabled(PIO1_IRQ_0, true);
    pio_set_irq0_source_enabled(CMD_PIO, pis_interrupt0, true);*/

    uint8_t i = 0;
    gpio_put(LED_PIN, false);
    while (true) {
        gpio_put(LED_PIN, !gpio_get(LED_PIN));
        sleep_us(500000);
        pio_sm_put_blocking(CMD_PIO, CMD_SM, ++i);
        printf("Response : %x\n", gpioToByte(pio_sm_get_blocking(CMD_PIO, CMD_SM)));
        sleep_us(5000000);
        printf("Command done");
        pio_sm_exec(CMD_PIO, CMD_SM, pio_encode_set(pio_x, 1));
    }
    return 0;
}
