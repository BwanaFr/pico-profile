#include "io_functions.h"

void initializeGPIO()
{
    //Initializes GPIO for bus output
    gpio_init(OD0);
    gpio_init(OD1);
    gpio_init(OD2);
    gpio_init(OD3);
    gpio_init(OD4);
    gpio_init(OD5);
    gpio_init(OD6);
    gpio_init(OD7);
    gpio_set_dir(OD0, GPIO_OUT);
    gpio_set_dir(OD1, GPIO_OUT);
    gpio_set_dir(OD2, GPIO_OUT);
    gpio_set_dir(OD3, GPIO_OUT);
    gpio_set_dir(OD4, GPIO_OUT);
    gpio_set_dir(OD5, GPIO_OUT);
    gpio_set_dir(OD6, GPIO_OUT);
    gpio_set_dir(OD7, GPIO_OUT);
    //GPIO bus input
    gpio_init(ID0);
    gpio_init(ID1);
    gpio_init(ID2);
    gpio_init(ID3);
    gpio_init(ID4);
    gpio_init(ID5);
    gpio_init(ID6);
    gpio_init(ID7);
    gpio_set_dir(ID0, GPIO_IN);
    gpio_set_dir(ID1, GPIO_IN);
    gpio_set_dir(ID2, GPIO_IN);
    gpio_set_dir(ID3, GPIO_IN);
    gpio_set_dir(ID4, GPIO_IN);
    gpio_set_dir(ID5, GPIO_IN);
    gpio_set_dir(ID6, GPIO_IN);
    gpio_set_dir(ID7, GPIO_IN);	

    //Control signals
    gpio_init(BSY);
    gpio_set_dir(BSY, GPIO_OUT);

    gpio_init(RW);
    gpio_set_dir(RW, GPIO_IN);

    gpio_init(CMD);
    gpio_set_dir(CMD, GPIO_IN);

    gpio_init(STRB);
    gpio_set_dir(STRB, GPIO_IN);
}