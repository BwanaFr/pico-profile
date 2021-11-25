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
    multicore_lockout_victim_init();
    HMI::initialize();
    while (true) {
        HMI::handleHMI();
    }
}

static DC42File file;
static Protocol proto(&file);

int main() {
    stdio_init_all();
    initializeGPIO();
    //Starts display on second core
    multicore_launch_core1(core1_entry);
    printf("\nPico-profile\n");
    if(!file.open("lisaem-profile.dc42")){
        printf("Unable to open file : %s\n", file.getLastError());
    }
    proto.initialize();
    while (true) {
        proto.handleProtocol();
    }
    return 0;
}
