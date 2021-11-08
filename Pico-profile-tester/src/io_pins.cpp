#include <Arduino.h>
#include "io_pins.h"

void configureIO()
{
    //Configure bus as input
    pinMode(PD0, INPUT);
    pinMode(PD1, INPUT);
    pinMode(PD2, INPUT);
    pinMode(PD3, INPUT);
    pinMode(PD4, INPUT);
    pinMode(PD5, INPUT);
    pinMode(PD6, INPUT);
    //Strobe is an input on profile side
    pinMode(PSTRB, OUTPUT);
    //Busy is from profile to this
    pinMode(PBSY, INPUT);
    //Command is from this to profile
    pinMode(PCMD, OUTPUT);
    //Parity is from profile to this
    pinMode(PPARITY, INPUT);
    //Read/write is from this to profile
    pinMode(PRW, OUTPUT);
    //Reset is from this to profile
    pinMode(RESET, OUTPUT);
    //LED for tests
    pinMode(LED_BUILTIN, OUTPUT);

    //Set direction as read
    digitalWrite(PRW, 0);
    //Don't reset the pico
    digitalWrite(RESET, 1);
    //No commands yet
    digitalWrite(PCMD, 1);
    //Keep strobe high
    digitalWrite(PSTRB, 1);
}

void setPortDir(bool output, uint8_t outValue = 0)
{
    if(output)
    {
        uint8_t oldSREG = SREG;
        cli();
        //Write output
        PORT10 = (PORT10 & B11111100) | (outValue & B00000011);
        PORT27 = (PORT27 & B00000011) | (outValue & B11111100);
        //Sets DDR
        DDR10 |= B00000011;
        DDR27 |= B11111100;
        SREG = oldSREG;
    }else{
        uint8_t oldSREG = SREG;
        cli();
        //Sets DDR
        PORT10 &= ~B00000011;
        PORT27 &= ~B11111100;
        DDR10 &= ~B00000011;
        DDR27 &= ~B11111100;
        SREG = oldSREG;
    }
}

uint8_t readBus()
{
    return (PIN10 & B00000011) | (PIN27 & B11111100);
}

uint8_t readData(bool useStrobe)
{
    //Set direction as read
    digitalWrite(PRW, 1);
    //Set port direction to read
    setPortDir(false);
    if(useStrobe){
        digitalWrite(PSTRB, false);
        //delayMicroseconds();
        digitalWrite(PSTRB, true);
    }
    uint8_t value = readBus();
    return value;
}

void writeData(uint8_t data, bool useStrobe)
{
    //Set port as output
    setPortDir(true, data);    
    //Set direction as write
    digitalWrite(PRW, 0);
    if(useStrobe){
        digitalWrite(PSTRB, false);
        digitalWrite(PSTRB, true);
    }
}
