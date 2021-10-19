#include <stdio.h>
#include "pico/stdlib.h"

#include "ff.h"
#include "sd_card.h"

FATFS fs;

static void run_unmount() {
    char *arg1 = "0:";
    FRESULT fr = f_unmount(arg1);
    if (FR_OK != fr) printf("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
}

static void run_mount() {
    char *arg1 = "0:";
    FATFS *p_fs = &fs;
    FRESULT fr = f_mount(p_fs, arg1, 1);
    if (FR_OK != fr) printf("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
}

static void run_getfree() {
    char *arg1 = "0:";
    DWORD fre_clust, fre_sect, tot_sect;
    /* Get volume information and free clusters of drive */
    FATFS *p_fs = &fs;
    FRESULT fr = f_getfree(arg1, &fre_clust, &p_fs);
    if (FR_OK != fr) {
        printf("f_getfree error: %s (%d)\n", FRESULT_str(fr), fr);
        return;
    }
    /* Get total sectors and free sectors */
    tot_sect = (p_fs->n_fatent - 2) * p_fs->csize;
    fre_sect = fre_clust * p_fs->csize;
    /* Print the free space (assuming 512 bytes/sector) */
    printf("%10lu KiB total drive space.\n%10lu KiB available.\n", tot_sect / 2,
           fre_sect / 2);
}

static void run_test_write(){
    FIL f;
    FRESULT fr;     /* FatFs return code */
    char line[10];
    UINT written;

    fr = f_open(&f, "message.txt", FA_READ);
    if(fr){
        return;
    }
    for(int i=0;i<20;++i){
        sprintf(line, "Line %d\n", (i+1));
        f_write(f, strlen(line), )
    }
}

int main() {
    stdio_init_all();
    sd_init_driver();
    sleep_ms(20000);
    printf("Hello, world!\n");
    sleep_ms(500);
    printf("Mounting SD card\n");
    run_mount();
    run_getfree();
    run_unmount();
    
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