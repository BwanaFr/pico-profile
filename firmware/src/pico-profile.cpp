#include <stdio.h>
#include "pico/stdlib.h"
#include "io_functions.hxx"
#include "protocol.hxx"
#include "DC42File.hxx"


int main() {
    stdio_init_all();
    initializeGPIO();
    DC42File file;
    for(uint32_t i=10;i>0;--i) {
        sleep_ms(500);
        printf("%lu ", i);
    }
    
    printf("\nPico-profile\n");
    if(!file.open("lisaem-profile.dc42")){
        printf("Unable to open file : %s\n", file.getLastError());
    }else{
        char name[64];
        if(file.readImageName(name))
            printf("DC42 image name : %s\n", name);
    }
    Protocol proto(&file);
    while (true) {
        proto.handleProtocol();
    }
    return 0;
}
