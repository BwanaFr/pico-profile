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
    //Load configuration file (or fallback to default value)
    sleep_ms(5000);
    ConfigFile::loadFile();
    //Starts display on second core
    multicore_launch_core1(core1_entry);
    printf("Pico-profile\n");
    if(!file.open(ConfigFile::getImageFileName())){
        std::string str = "Unable to open file ";
        str += ConfigFile::getImageFileName();
        str += " : ";
        str += file.getLastError();
        printf("%s\n", str.c_str());
        HMI::setFatalMsg(str.c_str());
        while (true) {
            tight_loop_contents();
        }
    }
    //Initialize protocol handling
    proto.initialize();
    while (true) {
        proto.handleProtocol();
    }
    return 0;
}
