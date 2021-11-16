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
  while(!digitalRead(PBSY)){}
  //Put bus to read and lower CMD
  readData(false);
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
    cmdBytes[1] = (block >> 16) & 0xFF;
    cmdBytes[2] = (block >> 8) & 0xFF;
    cmdBytes[3] = (block & 0xFF);
    cmdBytes[4] = retry;
    cmdBytes[5] = spare;
    Serial.println("Writing command");
    for(uint8_t i=0;i< 6;++i){
      writeData(cmdBytes[i]);
    }
}

void writePartialCommand(uint8_t cmd, uint32_t block) {                      
    uint8_t cmdBytes[4];
    cmdBytes[0] = cmd;
    cmdBytes[1] = (block >> 16) & 0xFF;
    cmdBytes[2] = (block >> 8) & 0xFF;
    cmdBytes[3] = (block & 0xFF);
    Serial.println("Writing partial command");
    for(uint8_t i=0;i<4;++i){
      writeData(cmdBytes[i]);
    }
}

static uint32_t readBlock = 0x0;

bool checkParityBit(uint8_t data, bool parity)
{
    int count = 0;
    for(int i=0;i<8;++i){
        if((data>>i) & 0x1){
            ++count;
        }
    }
    bool even = ((count % 2) == 0);
    return even == parity;
}

void doRead(uint32_t block, bool handshake, bool parity, bool benchmark){
  if(handshake) {
    uint8_t resp = 0;
    Serial.println("Performing read");
    resp = performCmdHandshake();
    if(resp != 0x1){
      Serial.print("Profile not ready! -> ");
      Serial.println(resp);
      return;
    }
    //Write read command
    writeCommand(0x0, block, 0x2, 0x3);
    //Wait for confirmation
    resp = performCmdHandshake();
    if(resp != 2){
      Serial.print("Unexpected Profile response (0x2) -> ");
      Serial.println(resp);
      return;
    }
  }
  uint32_t start = millis();
  //Read complete response
  uint8_t response[536];
  if(parity) {
    for(int i=0;i<536;++i){
      bool parity = false;
      response[i] = readDataParity(parity);
      if(!checkParityBit(response[i], parity)){
        Serial.println("Parity error!");
      }
    }
  }else {
    for(int i=0;i<536;++i){
      response[i] = readData();
    }
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
    
    //20 bytes tag
    Serial.print("Tag : ");
    for(int i=0;i<20;++i){
      Serial.print(" 0x");
      Serial.print(response[loc++], HEX);
    }
    Serial.println();

    //Read 512 block data
    int count = 0;
    while(count < 512){
      Serial.print(count, HEX);
      Serial.print("\t : ");
      for(int i=0;i<16;++i){
        Serial.print(" ");
        if(response[loc]<0x10){
          Serial.print("0");
        }
        Serial.print(response[loc], HEX);
        ++count;
        ++loc;
      }
      Serial.println();
    }

  }
  Serial.print("Read cycle done in ");
  Serial.print(elapsed);
  Serial.println("ms.");
}

void doWrite(uint32_t block) {
  uint8_t resp = 0;
  Serial.println("Performing write");
  resp = performCmdHandshake();
  if(resp != 0x1) {
    Serial.print("Profile not ready -> ");
    Serial.println(resp);
    return;
  }
  //Write write command
  writeCommand(0x1, block, 0x2, 0x3);
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

void doSpare(bool partialCmd) {
  uint8_t resp = 0;
  Serial.println("Performing spare table read");
  resp = performCmdHandshake();
  if(resp == 0x1){
    //Write read command
    if(partialCmd){
      writePartialCommand(0x0, 0xffffff);
    }else{      
      writeCommand(0x0, 0xffffff, 0x2, 0x3);
    }
    //Wait for confirmation
    resp = performCmdHandshake();
    if(resp != 2){
      Serial.print("Unexpected Profile response (0x2) -> ");
      Serial.println(resp);
      return;
    }
    uint32_t start = millis();
    //Read complete response
    uint8_t response[32];
    for(int i=0;i<32;++i){
      response[i] = readData();
    }
    uint32_t elapsed = millis() - start;
    int loc = 4;
    int count = 0;
    while(count < 32){
      Serial.print(count, HEX);
      Serial.print("\t : ");
      for(int i=0;i<16;++i){
        Serial.print(" ");        
        if(response[loc]<0x10){
          Serial.print("0");
        }
        Serial.print(response[loc], HEX);
        ++loc;
        ++count;
      }
      Serial.println();
    }

    Serial.print("Read cycle done in ");
    Serial.print(elapsed);
    Serial.println("ms.");
    Serial.print("Profile name :");
    Serial.println((char*)&response[4]);
  }else{
    Serial.print("Profile not ready! -> ");
    Serial.println(resp);
  } 
}

static uint32_t ramData = 0x0;
void doWriteRAM() {
  uint8_t resp = 0;
  Serial.println("Performing write");
  resp = performCmdHandshake();
  if(resp != 0x1) {
    Serial.print("Profile not ready -> ");
    Serial.println(resp);
    return;
  }

  Serial.println("Writing command");
  writePartialCommand(0x1, 0xFFFFFE);
  resp = performCmdHandshake();
  if(resp != 0x3) {
    Serial.print("Profile write not ack (0x3) -> ");
    Serial.println(resp);
    return;
  }
  //Send data
  writeData((ramData>>24) & 0xFF);
  writeData((ramData>>16) & 0xFF);
  writeData((ramData>>8) & 0xFF);
  writeData(ramData & 0xFF);
  ++ramData;
  for(uint16_t i=4;i<532;++i){
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
    doRead(readBlock++, true, false, true);    
    break;
  case 't':
    doSpare(true);
    break;
  case 'w':
    doWrite(readBlock);
    break;
  case 'c':
    writeCommand(0x0, 0x1, 0x20, 0x30);
    break;
  case 'd':
    doWriteRAM();
    //doRead(0xfffffe, true, false, false); 
    break;
  case 's':
    doSpare(true);
    break;
  case 'x':
    doRead(0xfffffe, true, false, false); 
    break;
  case 'p':
    Serial.println("Checking parity");
    for(uint8_t i=0;i<255;++i){
      writeData(i, false);
      if(!checkParity(i)){
        Serial.print("Parity error for data 0x");
        Serial.println(i);
      }
    }
  }

}