#include <stdio.h>
#include "pico/stdlib.h"

#include "pico-profile.pio.h"
#include "io_functions.h"
#include "ff.h"
#include "sd_card.h"

// FATFS fs;

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

uint8_t gpioToByte(uint32_t value) {
    return (value & 0x7F) | ((value >> 4) & 0x80);
}


int main() {
    stdio_init_all();
    initializeGPIO();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    printf("Pico-profile\n");
    PIO pio = pio0;
    uint sm = 0;
    uint pio_prog_offs = pio_add_program(pio, &pico_profile_data_program);
    pico_profile_data_pio_init(pio, sm, pio_prog_offs);
    
    while (true) {
        gpio_put(LED_PIN, !gpio_get(LED_PIN));
        uint32_t data = pio_sm_get_blocking(pio, sm);
        printf("Data : 0x%lx\n", gpioToByte(data));
        /*gpio_put(LED_PIN, 0);
        gpio_put(BSY, 0);
        sleep_ms(500);
        gpio_put(LED_PIN, 1);
        gpio_put(BSY, 1);
        sleep_ms(100);
        while(gpio_get(STRB)){}
        gpio_put(LED_PIN, 0);
        while(!gpio_get(STRB)){}*/
        //gpio_put(LED_PIN, gpio_get(CMD));
        /*while(gpio_get(RW)){}
        gpio_put(LED_PIN, 0);        
        putToBus(++i);
        printf("GPIO[%u] : ->", i);
        while(!gpio_get(RW)){}
        uint8_t rd = getBus();
        printf("%u Diff : %d\n", rd, (int8_t)(i-rd));        
        gpio_put(LED_PIN, 1);*/
    }
    /*sd_init_driver();
    sleep_ms(20000);
    printf("Hello, world!\n");
    sleep_ms(500);
    printf("Mounting SD card\n");
    run_mount();
    run_getfree();
    run_unmount();*/
    
    /*const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        printf("Hello, world!\n");
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
    }*/
    return 0;
}
