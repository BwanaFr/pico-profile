#ifndef PROTOCOL_H_
#define PROTOCOL_H_

//Profile commands
#define READ_CMD 0
#define WRITE_CMD 1
#define WRITE_VERIFY_CMD 2
#define WRITE_SPARE 3

//Profile spare table address
#define SPARE_TABLE_ADDR 0xFFFFFF

enum ProfileResponse {  
    GET_COMMAND = 1,        //Get command
    READ_BLOCK,             //Read data block
    RCV_WRITE_DATA,         //Receive, write data block
    RCV_WRITE_VERIFY_DATA,  //Receive, write/verify data block
    DO_WRITE,               //Do actual write or write/verify on disk
};

enum ProfileState {
    IDLE,                   //Profile is waiting for command
    READ,                   //Profile is reading from disk
    WRITE,                  //Profile is writing data to disk
};
#endif