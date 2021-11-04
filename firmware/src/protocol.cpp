#include "protocol.hxx"
#include "DC42File.hxx"
#include <stdio.h>
#include "hardware/irq.h"

//PIO for command handshake
#define CMD_PIO pio0
#define DATA_PIO pio0

Protocol* singleton = nullptr;

void data_isr(){
    if(pio_interrupt_get(DATA_PIO, pis_sm0_rx_fifo_not_empty)) {
        printf("Data available\n");
        singleton->dataReceived();
    }else{
        //Data TX FIFO
        printf("Data send\n");
    }
    irq_clear(PIO0_IRQ_0);
}

Protocol::Protocol(DC42File* file) :
    file_(file), state_(ProfileState::GET_COMMAND),
    received_(0), toSend_(0), status_(0)
{
    singleton = this;
    //Initialize PIO state machines
    pioDataOffs_ = pio_add_program(DATA_PIO, &pico_profile_data_program);
    pioDataCfg_ = pico_profile_data_pio_init(DATA_PIO, DATA_SM, pioDataOffs_);
    //pioCmdHShkOffs_ = pio_add_program(CMD_PIO, &pico_profile_cmd_hshk_program);
    //pioCmdHShkCfg_ = pico_profile_cmd_hshk_pio_init(CMD_PIO, CMD_SM, pioCmdHShkOffs_);
    //switchState(ProfileState::GET_COMMAND);
    /*irq_set_exclusive_handler(PIO0_IRQ_0, data_isr);
    irq_set_enabled(PIO0_IRQ_0, true);
    pio_set_irq0_source_enabled(DATA_PIO, pis_sm0_rx_fifo_not_empty, true);*/
    //pio_set_irq0_source_enabled(DATA_PIO, pis_sm0_tx_fifo_not_full, true);    
}

Protocol::~Protocol() {
}

void Protocol::switchState(ProfileState newState) {
    //Put next state in FIFO. The PIO will put
    //when CMD goes high
    pio_sm_put_blocking(CMD_PIO, CMD_SM, newState);
    state_ = newState;
}

void Protocol::handleProtocol() {
    printf("Received : %x\n", gpioToByte(pio_sm_get_blocking(DATA_PIO, DATA_SM)));
    /*bool ackReceived = false;
    if(pio_sm_get_rx_fifo_level(CMD_PIO, CMD_SM) != 0){
        //Handshake command completed (cmd lowered)
        uint8_t resp = gpioToByte(pio_sm_get_blocking(CMD_PIO, CMD_SM));
        printf("Apple response : %x\n", resp);
        if(resp != APPLE_ACK){
            setStatus(STATUS_55_NOT_RECEIVED);
        }else{
            resetStatus(STATUS_55_NOT_RECEIVED);
        }
        ackReceived = true;
        //Prepare to receive bunch of data
        resetDataStateMachine();
    }
    switch (state_)
    {
    case ProfileState::GET_COMMAND:
        manageCommand(ackReceived);                
        break;
    case ProfileState::READ_BLOCK:
        manageRead(ackReceived);
    default:
        break;
    }*/
}

void Protocol::manageCommand(bool ackReceived) {
    if(ackReceived) {
        //Lower the busy line
        sleep_ms(1000);
        handshakeDone();
        //Received enough bytes for command
        for(int i=0;i<6;++i){
            buffer_[i] = gpioToByte(pio_sm_get_blocking(DATA_PIO, DATA_SM));
            printf("Received : %x", buffer_[i]);
            received_++;
        }
    }
    if(received_ >= CMD_LENGTH){
        lastCmd_.command = static_cast<ProfileCommand>(buffer_[0]);
        lastCmd_.blockNumber = buffer_[1] << 16 | buffer_[2] << 8 | buffer_[3];
        lastCmd_.retryCount = buffer_[4];
        lastCmd_.sparesThreshold = buffer_[5];
        printCommand(lastCmd_);
        //Change state depending on command
        switch(lastCmd_.command){
            case ProfileCommand::READ:
                //Read command
                switchState(ProfileState::READ_BLOCK);                
            break;
            case ProfileCommand::WRITE:
                //Write command
                switchState(ProfileState::RCV_WRITE_DATA);
            break;

        }
    }
}

void Protocol::manageRead(bool ackReceived) {
    if(ackReceived){
        readToBuffer();
        //Lower the busy line
        handshakeDone();
    }
    if(toSend_ < 0){
        printf("Read done\n");
        switchState(ProfileState::GET_COMMAND);
    }
}

void Protocol::readToBuffer() {
    //Read datablocks from file at offset 4
    //First bytes will be the status bit
    if(!file_->readBlock(lastCmd_.blockNumber, buffer_+4)){
        //Read failure, need to raise some status bits
        //TODO: If failure, does the apple read all bytes?
        setStatus(STATUS_UNSUCCESS);
    }
    if(!file_->readTag(lastCmd_.blockNumber, buffer_+4+512)){
        setStatus(STATUS_UNSUCCESS);
    }
    //Build status bytes
    buffer_[0] = status_ & 0xFF;
    buffer_[1] = (status_>>8) & 0xFF;
    buffer_[3] = (status_>>16) & 0xFF;
    buffer_[4] = (status_>>24) & 0xFF;
    //Send first byte
    toSend_ = BUFFER_SIZE-1;
    pio_sm_put_blocking(DATA_PIO, DATA_SM, buffer_[0]);
}

void Protocol::handshakeDone() {
    //The handshake state machine will loop until
    //the X scratch register is not 0
    pio_sm_exec(CMD_PIO, CMD_SM, pio_encode_set(pio_x, 1));
}

void Protocol::resetDataStateMachine() {
    //Restart the data state machine
    pio_sm_restart(DATA_PIO, DATA_SM);
    received_ = 0;
}

void Protocol::dataReceived() {
    if(toSend_ == 0) {
        if(received_ < BUFFER_SIZE){
            buffer_[received_++] = pio_sm_get_blocking(DATA_PIO, DATA_SM);
        }else{
            printf("Received too much data\n");
        }
    }else{
        printf("Received data but data send in progress\n");
    }
}

void Protocol::setStatus(uint32_t bits) {
    status_ |= bits;
}

void Protocol::resetStatus(uint32_t bits) {
    status_ &= ~bits;
}

/**
 * Utility to print command details to serial
 */
void Protocol::printCommand(const CommandMessage& msg) {    
    switch (msg.command)
    {
    case ProfileCommand::READ:
        printf("Read block $%lx, retries:%u, spare threshold:%u\n", msg.blockNumber, 
                                                    msg.retryCount, msg.sparesThreshold);
        break;
    case ProfileCommand::WRITE:
        printf("Write block $%lx\n", msg.blockNumber);
        break;
    case ProfileCommand::WRITE_VERIFY:
        printf("Write & verify block $%lx, retries:%u, spare threshold:%u\n", msg.blockNumber, 
                                                    msg.retryCount, msg.sparesThreshold);
        break;
    case ProfileCommand::WRITE_SPARE:
        printf("Write & Force Sparing block $%lx\n", msg.blockNumber);
        break;
    default:
        printf("Unsupported command\n");
        break;
    }
    
}