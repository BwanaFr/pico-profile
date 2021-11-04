#include <stdio.h>
#include "pico/stdlib.h"
#include "io_functions.hxx"
#include "protocol.hxx"
#include "DC42File.hxx"


int main() {
    stdio_init_all();
    initializeGPIO();
    DC42File file;
    for(uint32_t i=0;i<15;++i) {
        sleep_ms(500);
        printf("%u ", i);
    }
    Protocol proto(&file);
    printf("\nPico-profile\n");
    if(!file.open("lisaem-profile.dc42")){
        printf("Unable to open file : %s\n", file.getLastError());
    }else{
        char name[64];
        if(file.readImageName(name))
            printf("DC42 image name : %s\n", name);
    }

    while (true) {
        proto.handleProtocol();
    }

    //Initialize the command state machine
    //pio_cmd_offs = pio_add_program(CMD_PIO, &pico_profile_cmd_program);
    //pio_cmd_cfg = pico_profile_cmd_pio_init(CMD_PIO, CMD_SM, pio_cmd_offs);
    //Cmd handshake
    //pio_cmd_offs = pio_add_program(CMD_PIO, &pico_profile_cmd_hchk_program);
    //pio_cmd_cfg = pico_profile_cmd_hchk_pio_init(CMD_PIO, CMD_SM, pio_cmd_offs);
    /*irq_set_exclusive_handler(PIO1_IRQ_0, isr);
    irq_set_enabled(PIO1_IRQ_0, true);
    pio_set_irq0_source_enabled(CMD_PIO, pis_interrupt0, true);*/
/*
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
    */
    return 0;
}
