#ifndef IO_PINS_H_
#define IO_PINS_H_
#include <Arduino.h>

//Data port
#define P_D0 14
#define P_D1 15
#define P_D2 2
#define P_D3 3
#define P_D4 4
#define P_D5 5
#define P_D6 6
#define P_D7 7

//DDR and PORT definitions
#define DDR10 DDRC
#define DDR27 DDRD
#define PORT10 PORTC
#define PORT27 PORTD
#define PIN10 PINC
#define PIN27 PIND

//Stobe output
#define PSTRB 8
//Profile busy input
#define PBSY 9
//Command output
#define PCMD 10
//Parity input
#define PPARITY 11
//Read/write output
#define PRW 12
//Profile reset output
#define RESET 16

/**
 * Performs the initial configuration of IO
 */
void configureIO();

/**
 * Cycles the stobe line
 **/
void cycleStrobe();

/**
 * Reads data from profile
 * @param useStrobe If true raise the strobe line
 * @return data read from bus
 */
uint8_t readData(bool useStrobe = true);

/**
 * Reads data and parity bit from profile
 * @param parity Parity bit (true if even)
 * @param useStrobe If true raise the strobe line
 * @return  data read from bus
 */
uint8_t readDataParity(bool& parity, bool useStrobe = true);

/**
 * Writes data to profile
 * @param data Data to be written
 */
void writeData(uint8_t data, bool useStrobe = true);

/**
 * Checks the parity bit
 * @param data Data to be parity-check
 * @return True if parity matches
 **/
bool checkParity(uint8_t data);

#endif