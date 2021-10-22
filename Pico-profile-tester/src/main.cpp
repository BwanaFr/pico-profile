#include <Arduino.h>
#include "io_pins.h"

void setup() {
  Serial.begin(115200);
  configureIO();
}

uint8_t i=0;

void loop() {
  writeData(++i, true);
  Serial.print('.');
  _delay_ms(100);
  //digitalWrite(LED_BUILTIN, digitalRead(PBSY));
  /*uint8_t data =  readData(true);
  Serial.println(data);
  _delay_ms(500);
  writeData(data+1, true);
  _delay_ms(50);
  */
  /*digitalWrite(PRW, 0);
  Serial.print(" 0:");
  Serial.println(readData());
  _delay_ms(500);*/
  /*digitalWrite(PRW, 0);
  Serial.print(" 1:");
  Serial.println(readData());
  _delay_ms(50);*/
 
}