#include "protocol.hxx"
#include "DC42File.hxx"
#include <stdio.h>
#include "hardware/irq.h"
#include <string.h>

//PIO for command handshake
#define CMD_PIO pio0
#define DATA_PIO pio0

#define PROFILE_DEBUG 1

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

void cmd_isr(){
    printf("CMD received");
    pio_interrupt_clear(CMD_PIO, 0);
    //irq_clear(PIO0_IRQ_0);
}

Protocol::Protocol(DC42File* file) :
    file_(file), state_(ProfileState::GET_COMMAND),
    received_(0), toSend_(0), status_(0)
{
    singleton = this;
    
    //Initialize PIO state machines
    configurePIO();    
    switchState(state_);
    updateSpareTable();
}

Protocol::~Protocol() {
}

void Protocol::switchState(ProfileState newState) {
    //Put next state in FIFO. The PIO will put
    //when CMD goes high
    printf("Switching state to : %u\n", newState);
    pio_sm_put_blocking(CMD_PIO, CMD_SM, newState);
    state_ = newState;
}

void Protocol::configurePIO() {
    uint32_t modify = 0;
    uint32_t dir = 0;

    modify |= 1u << OD0_PIN;
    modify |= 1u << OD1_PIN;
    modify |= 1u << OD2_PIN;
    modify |= 1u << OD3_PIN;
    modify |= 1u << OD4_PIN;
    modify |= 1u << OD5_PIN;
    modify |= 1u << OD6_PIN;
    modify |= 1u << OD7_PIN;
    modify |= 1u << BSY_PIN;
    modify |= 1u << RW_PIN;
    modify |= 1u << ID0_PIN;
    modify |= 1u << ID1_PIN;
    modify |= 1u << ID2_PIN;
    modify |= 1u << ID3_PIN;
    modify |= 1u << ID4_PIN;
    modify |= 1u << ID5_PIN;
    modify |= 1u << ID6_PIN;
    modify |= 1u << ID7_PIN;
    modify |= 1u << CMD_PIN;
    modify |= 1u << STRB_PIN;
    
    dir |= 1u << OD0_PIN;
    dir |= 1u << OD1_PIN;
    dir |= 1u << OD2_PIN;
    dir |= 1u << OD3_PIN;
    dir |= 1u << OD4_PIN;
    dir |= 1u << OD5_PIN;
    dir |= 1u << OD6_PIN;
    dir |= 1u << OD7_PIN;
    dir |= 1u << BSY_PIN; 
    for(uint32_t i=0;i<32;++i){
        if((modify>>i) & 0x1){
            pio_gpio_init(DATA_PIO, i);
        }
    }
    //Initialises the data state machine
    pioDataOffs_ = pio_add_program(DATA_PIO, &pico_profile_data_program);
    pioDataCfg_ = pico_profile_data_program_get_default_config(pioDataOffs_);
    sm_config_set_out_pins(&pioDataCfg_, 0, 8);                 //8 outputs
    sm_config_set_out_shift(&pioDataCfg_, true, true, 8);       //Shift right, autopull of 8 bits
    sm_config_set_in_pins(&pioDataCfg_, ID0_PIN);               //Input starting at ID0_PIN (GPIO16)
    sm_config_set_in_shift(&pioDataCfg_, false, true, 11);      //Shift right, autopull of 11 bits (to take into account GPIO 26)
    sm_config_set_jmp_pin(&pioDataCfg_, RW_PIN);                //Use R/W as jump pin

    pio_sm_init(DATA_PIO, DATA_SM, pioDataOffs_, &pioDataCfg_); //Initialize state machine
    pio_sm_set_pindirs_with_mask(DATA_PIO, DATA_SM, dir, modify);
    pio_sm_set_enabled(DATA_PIO, DATA_SM, false);                //Enable state machine

    //Initializes the command handshake PIO
    pioCmdOffs_ = pio_add_program(CMD_PIO, &pico_profile_cmd_program);
    pioCmdCfg_ = pico_profile_cmd_program_get_default_config(pioCmdOffs_);
    sm_config_set_out_pins(&pioCmdCfg_, 0, 8);                  //8 outputs
    sm_config_set_out_shift(&pioCmdCfg_, true, true, 8);        //Shift right, autopull of 8 bits
    sm_config_set_sideset_pins(&pioCmdCfg_, BSY_PIN);           //Busy pin (side-set) is GPIO8
    sm_config_set_in_pins(&pioCmdCfg_, ID0_PIN);                //Input starting at GPIO16
    sm_config_set_in_shift(&pioCmdCfg_, false, true, 11);       //Shift right, autopull of 11 bits (to take into account GPIO 26)

    pio_sm_init(CMD_PIO, CMD_SM, pioCmdOffs_, &pioCmdCfg_);    //Initialize state machine
    pio_sm_set_pindirs_with_mask(CMD_PIO, CMD_SM, dir, modify);
    pio_sm_set_enabled(CMD_PIO, CMD_SM, true);                      //Enable state machine
    irq_set_exclusive_handler(PIO0_IRQ_0, cmd_isr);                 //Set IRQ handler for new command
    irq_set_enabled(PIO0_IRQ_0, true);                              //Enable IRQ
    pio_set_irq0_source_enabled(CMD_PIO, pis_interrupt0, true);     //IRQ0 
}

void Protocol::handleProtocol() {
    bool ackReceived = false;
    if(pio_sm_get_rx_fifo_level(CMD_PIO, CMD_SM) != 0){
        //Handshake command completed (cmd lowered)
        uint8_t resp = gpioToByte(pio_sm_get_blocking(CMD_PIO, CMD_SM));
        printf("[%d] Apple response : 0x%x\n", state_, resp);
        if(resp != APPLE_ACK){
            setStatus(STATUS_55_NOT_RECEIVED);
            //Lower the busy line
            handshakeDone();
            switchState(ProfileState::GET_COMMAND);
            return;
        }else{
            resetStatus(STATUS_55_NOT_RECEIVED);
        }
        ackReceived = true;
        //Prepare to receive bunch of data
        reInitDataStateMachine();
    }
    
    switch (state_)
    {
    case ProfileState::GET_COMMAND:
        manageCommand(ackReceived);                
        break;
    case ProfileState::READ_BLOCK:
        manageRead(ackReceived);
        break;
    case ProfileState::RCV_WRITE_DATA:
        manageReceiveWriteData(ackReceived);
        break;
    case ProfileState::DO_WRITE:
        performWrite(ackReceived);
        break;
    default:
        break;
    }
}

void Protocol::manageCommand(bool ackReceived) {
    if(ackReceived) {
        //Lower the busy line
        handshakeDone();
        printf("Receiving command bytes... ");
        absolute_time_t expiryTime = make_timeout_time_ms(200);        
        //Received enough bytes for command
        for(int i=0;i<6;++i){
            bool dataReady = true;
            while(pio_sm_is_rx_fifo_empty(DATA_PIO, DATA_SM)){
                if(absolute_time_diff_us(expiryTime, get_absolute_time())>0){
                    printf("No command bytes received in 200ms.\n");
                    dataReady = false;
                    break;
                }
            }
            if(!dataReady){
                break;
            }
            buffer_[i] = gpioToByte(pio_sm_get_blocking(DATA_PIO, DATA_SM));
            printf(" 0x%x", buffer_[i]);
            received_++;
        }
        printf("\n");
    
        if(received_ >= CMD_LENGTH-2){
            lastCmd_.command = static_cast<ProfileCommand>(buffer_[0]);
            lastCmd_.blockNumber = buffer_[1] << 16 | buffer_[2] << 8 | buffer_[3];
            lastCmd_.retryCount = received_ >=4 ? buffer_[4] : 0x0;
            lastCmd_.sparesThreshold = received_ >=5 ? buffer_[5] : 0x0;
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
                default:
                    switchState(ProfileState::GET_COMMAND);
            }
        }else{
            switchState(ProfileState::GET_COMMAND);
        }
    }
}

void Protocol::manageRead(bool ackReceived) {
    if(ackReceived){
        readToBuffer();
        //Lower the busy line
        handshakeDone();
        reInitDataStateMachine();
        //Send data
        int i=0;
        absolute_time_t expiryTime = make_timeout_time_ms(100);
        printf("Sending data to FIFO\n");
        while(toSend_ > 0) {
            if(pio_sm_is_tx_fifo_full(DATA_PIO, DATA_SM)){
                int64_t timeDiff = absolute_time_diff_us(expiryTime, get_absolute_time());
                if(timeDiff>0){
                    printf("No send since 100ms giving up...\n");
                    reInitDataStateMachine();
                    break;
                }
            }else{
                pio_sm_put_blocking(DATA_PIO, DATA_SM, buffer_[i]);
                printf(".");
                ++i;
                --toSend_;
            }
        }
        printf("\nRead %d bytes\n", i);
        switchState(ProfileState::GET_COMMAND);
    }
    /*if(toSend_ <= 0){
        printf("Read done\n");
        switchState(ProfileState::GET_COMMAND);
    }*/
}

void Protocol::manageReceiveWriteData(bool ackReceived) {
    if(ackReceived) {
        //Lower the busy line (we should be ready to consume data)
        handshakeDone();
        //Get data
        for(int i=0;i<512;++i){
            buffer_[i+4] = gpioToByte(pio_sm_get_blocking(DATA_PIO, DATA_SM));
        }
        //Get tag
        for(int i=516;i<536;++i){
            buffer_[i] = gpioToByte(pio_sm_get_blocking(DATA_PIO, DATA_SM));
        }
        printf("Receive block $%lx completed\n", lastCmd_.blockNumber);
        dumpBuffer(false);
        switchState(ProfileState::DO_WRITE);
    }
}

void Protocol::performWrite(bool ackReceived) {
    if(ackReceived){
        printf("Writing to disk, block $%lx\n", lastCmd_.blockNumber);
        if (lastCmd_.blockNumber<0x00f00000) lastCmd_.blockNumber=deinterleave5(lastCmd_.blockNumber);
        file_->writeBlock(lastCmd_.blockNumber, &buffer_[4]);
        file_->writeTag(lastCmd_.blockNumber, &buffer_[516]);
        handshakeDone();
        for(uint8_t i=0;i<4;++i){
            pio_sm_put_blocking(DATA_PIO, DATA_SM, 0x0);
        }
        switchState(ProfileState::GET_COMMAND);
    }
}

void Protocol::readToBuffer() {
    //Read datablocks from file at offset 4
    //First bytes will be the status bit
    absolute_time_t start = get_absolute_time();
    if (lastCmd_.blockNumber<0x00f00000)   lastCmd_.blockNumber=deinterleave5(lastCmd_.blockNumber);
    if(lastCmd_.blockNumber != SPARE_TABLE_ADDR){
        if(!file_->readBlock(lastCmd_.blockNumber, buffer_+4)){
            //Read failure, need to raise some status bits
            //TODO: If failure, does the apple read all bytes?
            setStatus(STATUS_UNSUCCESS);
        }
        if(!file_->readTag(lastCmd_.blockNumber, buffer_+4+512)){
            setStatus(STATUS_UNSUCCESS);
        }
    }else{
        printf("Spare table size : %u\n", sizeof(SpareTable));
        memcpy(&buffer_[4], &spareTable_.name[0], sizeof(SpareTable));
        status_ = 0;
    }
    absolute_time_t end = get_absolute_time();
    printf("Time to read data : %lldus\n", absolute_time_diff_us(start, end));
    //Build status bytes
    buffer_[0] = status_ & 0xFF;
    buffer_[1] = (status_>>8) & 0xFF;
    buffer_[2] = (status_>>16) & 0xFF;
    buffer_[3] = (status_>>24) & 0xFF;

    //Read status bytes
#ifdef PROFILE_DEBUG
    dumpBuffer();
#endif
    //Send first byte
    toSend_ = BUFFER_SIZE;
}

void Protocol::dumpBuffer(bool status, bool data, bool tag) {
    int i = 0;
    if(status){
        printf("Status :");
        for(int j=0;j<4;++j) {
         printf(" 0x%x", buffer_[i++]);
        }
    }else{
        i = 4;
    }
    if(data) {
        //Read 512 block data
        while(i < 516){
            printf("\n%x\t", i-4);
            for(int j=0;j<32;++j){
                printf(" 0x%x", buffer_[i++]);
            }
        }
    }else{
        i = 516;
    }
    if(tag){
        printf("\nTag : ");
        for(int j=0;j<20;++j){
            printf(" 0x%x", buffer_[i++]);
        }
    }
    printf("\n");
}

void Protocol::updateSpareTable() {
    uint32_t blockCount = 0;
    if(!file_->getDataBlockCount(blockCount)){
        printf("Unable to get block count from file!\n");
    }else{
        printf("Disk has %u blocks.\n", blockCount);
    }
    switch (blockCount)
    {
     case 19456:  spareTable_.name[8]='1'; spareTable_.name[9]='0'; spareTable_.name[10]='M';    break;//10M
     case 32768:  spareTable_.name[8]='1'; spareTable_.name[9]='6'; spareTable_.name[10]='M';    break;//16M
     case 40960:  spareTable_.name[8]='2'; spareTable_.name[9]='0'; spareTable_.name[10]='M';    break;//20M
     case 65536:  spareTable_.name[8]='3'; spareTable_.name[9]='2'; spareTable_.name[10]='M';    break;//32M
     case 81920:  spareTable_.name[8]='4'; spareTable_.name[9]='0'; spareTable_.name[10]='M';    break;//40M
     case 131072: spareTable_.name[8]='6'; spareTable_.name[9]='4'; spareTable_.name[10]='M';    break;//64M
   //case 9728:
     default:     spareTable_.name[8]=' '; spareTable_.name[9]=' '; spareTable_.name[10]=' ';          // 5M
    }
    spareTable_.deviceNum[2] = (blockCount>9728) ? 0x10 : 0x0;
    spareTable_.numBlocks[0] = (blockCount >> 16) & 0xFF;
    spareTable_.numBlocks[1] = (blockCount >> 8) & 0xFF;
    spareTable_.numBlocks[2] = blockCount & 0xFF;
}

void Protocol::handshakeDone() {
    //The handshake state machine will loop until
    //the X scratch register is not 0
    pio_sm_exec(CMD_PIO, CMD_SM, pio_encode_set(pio_x, 1));
}

void Protocol::reInitDataStateMachine() {
    //Restart the data state machine
    //pio_sm_restart(DATA_PIO, DATA_SM);
    pio_sm_init(DATA_PIO, DATA_SM, pioDataOffs_, &pioDataCfg_);
    pio_sm_set_enabled(DATA_PIO, DATA_SM, true);
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