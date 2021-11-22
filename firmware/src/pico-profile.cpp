#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "io_functions.hxx"
#include "protocol.hxx"
#include "DC42File.hxx"
#include "display/hmi.hxx"
#include "pico/multicore.h"

/**
 * Use the core 1 for display and HMI
 **/
void core1_entry() {
    int i=0;
    HMI::initialize();
    while (true) {
        //HMI::handleHMI();
        sleep_ms(100);
        printf("#%d : Hello from core 1\n", ++i);
    }
}

int main() {
    stdio_init_all();
    initializeGPIO();
    //Starts display on second core
    multicore_launch_core1(core1_entry);
    //TODO : Remove C++ objects from stack and make all method static
    DC42File file;
    printf("\nPico-profile\n");
    if(!file.open("lisaem-profile.dc42")){
        printf("Unable to open file : %s\n", file.getLastError());
    }
    Protocol proto(&file);
    while (true) {
        proto.handleProtocol();
    }
    return 0;
}
