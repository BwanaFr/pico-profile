#ifndef IO_FUNCTIONS_H_
#define IO_FUNCTIONS_H_

#include "pico/stdlib.h"

//Output port
#define OD0 0
#define OD1 1
#define OD2 2
#define OD3 3
#define OD4 4
#define OD5 5
#define OD6 6
#define OD7 7
//Input port
#define ID0 16
#define ID1 17
#define ID2 18
#define ID3 19
#define ID4 20
#define ID5 21
#define ID6 22
#define ID7 26

//Control signals
#define BSY 8
#define RW 9
#define CMD 27
#define STRB 28

void initializeGPIO();

#endif