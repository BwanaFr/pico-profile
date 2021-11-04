#include <Arduino.h>
#include "io_pins.h"

void setup() {
  Serial.begin(115200);
  configureIO();
  digitalWrite(RESET, false);
  _delay_ms(100);
  digitalWrite(RESET, true);
}

uint8_t i = 0;

uint8_t performCmdHandshake(){
    //Lower CMD
  digitalWrite(PCMD, false);
  Serial.println("CMD low, wait for busy");
  while(digitalRead(PBSY)){}
  Serial.print("Busy low, reading data : 0x");
  uint8_t data = readData();
  Serial.println(data, HEX);
  Serial.println("Answering 0x55");
  writeData(0x55, false);
  digitalWrite(PCMD, true);
  Serial.println("Waiting for busy high");
  while(!digitalRead(PBSY)){}
  return data;
}

/**
 * Send a read request to profile
 **/
void performRead(){
  //Lower CMD
  digitalWrite(PCMD, false);
  Serial.println("CMD low, wait for busy");
  while(digitalRead(PBSY)){}
  Serial.print("Busy low, reading data : 0x");
  uint8_t data = readData();
  Serial.println(data, HEX);
  Serial.println("Answering 0x55");
  if(i == 0){
    writeData(0x55, false);
  }else{
    writeData(0x22, false);
  }
  //Put cmd high
  digitalWrite(PCMD, true);
  Serial.println("Waiting for busy high");
  while(!digitalRead(PBSY)){}
  Serial.println("Busy lowered, writing command bytes");
  _delay_us(200);
  for(uint8_t i=0;i<6;++i){
    writeData(i);
    _delay_us(16);
  }
  digitalWrite(PCMD, false);
  Serial.println("Waiting for BUSY to be low");
  while(digitalRead(PBSY)){}
  Serial.print("Busy low, answer : 0x");
  data = readData(false);
  Serial.println(data, HEX);
  Serial.println("Answering 0x55");
  writeData(0x55, false);
  //Put cmd high
  digitalWrite(PCMD, true);
  Serial.println("Waiting for BUSY to be high");
  while(!digitalRead(PBSY)){}
  Serial.println("Read done!");
}

void writeCommand(uint8_t cmd, uint32_t block,
                    uint8_t retry, uint8_t spare) {                      
    uint8_t cmdBytes[6];
    cmdBytes[0] = cmd;
    cmdBytes[1] = (block >> 24) & 0xFF;
    cmdBytes[2] = (block >> 16) & 0xFF;
    cmdBytes[3] = (block & 0xFF);
    cmdBytes[4] = retry;
    cmdBytes[5] = spare;
    Serial.println("Writing command");
    for(uint8_t i=0;i< 6;++i){
      writeData(cmdBytes[i]);
    }
}

void loop() {
  Serial.println("Hit a key to start reading");
  while(Serial.available() == 0){}
  while(Serial.available() != 0){Serial.read();}
  //uint8_t c = performCmdHandshake();
  //if(c == 0x1){
    writeCommand(0x2, 0x1, 0x20, 0x30);
  //  performCmdHandshake();
  //}
}