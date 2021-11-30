#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "io_functions.hxx"
#include "protocol.hxx"
#include "DC42File.hxx"
#include "display/hmi.hxx"
#include "pico/multicore.h"
#include "configFile.hxx"

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
    sleep_ms(5000);
    printf("Loading configuration file\n");
    absolute_time_t start = get_absolute_time();
    ConfigFile::loadFile();
    int diff = absolute_time_diff_us(start, get_absolute_time());
    printf("Time to read configuration file : %dus\n", diff);
    //Starts display on second core
    multicore_launch_core1(core1_entry);
    printf("\n----------------\nPico-profile\n");
    if(!file.open("lisaem-profile.dc42")){
        printf("Unable to open file : %s\n", file.getLastError());
    }
    proto.initialize();
    while (true) {
        proto.handleProtocol();
    }
    return 0;
}
