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
  Serial.print("Handshaking : ");
    //Lower CMD
  digitalWrite(PCMD, false);
//  Serial.println("CMD low, wait for busy");
  while(digitalRead(PBSY)){}
//  Serial.print("Busy low, reading data : 0x");
  uint8_t data = readData();  //Don't sure if strobe is triggered here
//  Serial.println(data, HEX);
//  Serial.println("Answering 0x55");
  writeData(0x55, false);
  digitalWrite(PCMD, true);
//  Serial.println("Waiting for busy high");
  while(!digitalRead(PBSY)){}
  Serial.println(data);
  return data;
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
static uint32_t readBlock = 0;
 
void doRead(bool benchmark){
  uint8_t resp = 0;
  Serial.println("Performing read");
  resp = performCmdHandshake();
  if(resp == 0x1){
    //Write read command
    writeCommand(0x0, readBlock++, 0x2, 0x3);
    //Wait for confirmation
    resp = performCmdHandshake();
    if(resp != 2){
      Serial.print("Unexpected Profile response (0x2) -> ");
      Serial.println(resp);
      return;
    }
    uint32_t start = millis();
    //Read complete response
    uint8_t response[536];
    for(int i=0;i<536;++i){
      response[i] = readData();
    }
    uint32_t elapsed = millis() - start;

    if(!benchmark){
      int loc = 0;
      //Read status bytes
      Serial.print("Status :");
      for(int i=0;i<4;++i) {
        Serial.print(" 0x");
        Serial.print(response[loc++], HEX);
      }
      Serial.println();
      //Read 512 block data
      int count = 0;
      while(count < 512){
        Serial.print(count, HEX);
        Serial.print("\t : ");
        for(int i=0;i<32;++i){
          Serial.print(" 0x");
          Serial.print(response[loc++], HEX);
          ++count;
        }
        Serial.println();
      }
      Serial.print("Tag : ");
      for(int i=0;i<20;++i){
        Serial.print(" 0x");
        Serial.print(response[loc++], HEX);
      }
      Serial.println();
    }
    Serial.print("Read cycle done in ");
    Serial.print(elapsed);
    Serial.println("ms.");
  }else{
    Serial.print("Profile not ready! -> ");
    Serial.println(resp);
  } 
}

void doWrite() {
  uint8_t resp = 0;
  Serial.println("Performing write");
  resp = performCmdHandshake();
  if(resp != 0x1) {
    Serial.print("Profile not ready -> ");
    Serial.println(resp);
    return;
  }
  //Write write command
  writeCommand(0x1, readBlock, 0x2, 0x3);
  resp = performCmdHandshake();
  if(resp != 0x3) {
    Serial.print("Profile write not ack (0x3) -> ");
    Serial.println(resp);
    return;
  }
  //Send data
  for(uint16_t i=0;i<532;++i){
    writeData(i & 0xFF);
  }
  resp = performCmdHandshake();
  if(resp != 0x6) {
    Serial.print("Profile write not ack (0x6) -> ");
    Serial.println(resp);
    return;
  }
  Serial.print("Status :");
  for(uint16_t i=0;i<4;++i){
    Serial.print(" 0x");
    Serial.print(readData(), HEX);
  }
  Serial.println("\nWrite done!");
}

void loop() {
  Serial.println("Hit a key to start reading");
  while(Serial.available() == 0){}
  char c = 0x0;
  while(Serial.available() != 0){c = Serial.read();}
  switch(c){
  case 'r':
    doRead(false);    
    break;
  case 't':
    doRead(true);    
    break;
  case 'w':
    doWrite();
    break;
  case 'c':
    writeCommand(0x0, 0x1, 0x20, 0x30);
    break;
  case 'd':
    for(int i=0;i<536;++i){
      writeData(i & 0xFF);
    }
    break;
  }
}