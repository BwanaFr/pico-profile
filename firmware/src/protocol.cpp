#include "protocol.hxx"
#include "DC42File.hxx"
#include <stdio.h>
#include "hardware/irq.h"
#include "hardware/dma.h"
#include <string.h>

//PIO for command handshake
#define CMD_PIO pio0
#define DATA_PIO pio0

#define PROFILE_DEBUG 1

#define RX_TO_BYTE(i) gpioToByte(rxBuffer_[i])

Protocol* singleton = nullptr;

/**
 * ISR when CMD line goes low
 * Called by the CMD state machine
 **/
void __isr cmd_isr() {
    singleton->commandReceived();
}

/**
 * Data DMA transfer completed (read or write)
 **/
void __isr data_dma_done() {
    singleton->dataDMADone();
}

Protocol::Protocol(DC42File* file) :
    file_(file),
    nextState_(ProfileState::GET_COMMAND),
    dataReadDMAChan_(-1), dataWriteDMAChan_(-1),
    received_(0), toSend_(0), status_(0)
{
    singleton = this;
    //Initialize semaphores
    sem_init(&dataWriteSem_, 0, 1); 
    sem_init(&dataReadSem_, 0, 1);
    //Initialize PIO state machines
    configurePIO();
    //Initial configuration of DMA channels
    configureDMA();
    //Updates the spare table depending on file
    updateSpareTable();
    //Sets the state machine to ready to accept commands
    setState(nextState_);
}

Protocol::~Protocol() {
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
    //Init GPIO
    for(uint32_t i=0;i<32;++i){
        if((modify>>i) & 0x1){
            pio_gpio_init(DATA_PIO, i);
        }
    }
    //Initialises the data read state machine
    pioDataReadOffs_ = pio_add_program(DATA_PIO, &pico_profile_data_read_program);
    pioDataReadCfg_ = pico_profile_data_read_program_get_default_config(pioDataReadOffs_);
    sm_config_set_out_pins(&pioDataReadCfg_, 0, 8);                 //8 outputs
    sm_config_set_out_shift(&pioDataReadCfg_, true, true, 8);       //Shift right, autopull of 8 bits
    sm_config_set_in_pins(&pioDataReadCfg_, ID0_PIN);               //Input starting at ID0_PIN (GPIO16)
    sm_config_set_in_shift(&pioDataReadCfg_, false, true, 11);      //Shift right, autopull of 11 bits (to take into account GPIO 26)
    sm_config_set_jmp_pin(&pioDataReadCfg_, RW_PIN);                //Use R/W as jump pin

    pio_sm_init(DATA_PIO, DATA_READ_SM, pioDataReadOffs_, &pioDataReadCfg_);     //Initialize state machine
    pio_sm_set_pindirs_with_mask(DATA_PIO, DATA_READ_SM, dir, modify);
    pio_sm_set_enabled(DATA_PIO, DATA_READ_SM, false);              //Don't enable state machine yet

    //Initialises the data write state machine
    pioDataWriteOffs_ = pio_add_program(DATA_PIO, &pico_profile_data_write_program);
    pioDataWriteCfg_ = pico_profile_data_write_program_get_default_config(pioDataWriteOffs_);
    sm_config_set_out_pins(&pioDataWriteCfg_, 0, 8);                 //8 outputs
    sm_config_set_out_shift(&pioDataWriteCfg_, true, true, 8);       //Shift right, autopull of 8 bits
    sm_config_set_in_pins(&pioDataWriteCfg_, ID0_PIN);               //Input starting at ID0_PIN (GPIO16)
    sm_config_set_in_shift(&pioDataWriteCfg_, false, true, 11);      //Shift right, autopull of 11 bits (to take into account GPIO 26)
    sm_config_set_jmp_pin(&pioDataWriteCfg_, RW_PIN);                //Use R/W as jump pin

    pio_sm_init(DATA_PIO, DATA_WRITE_SM, pioDataWriteOffs_, &pioDataWriteCfg_);     //Initialize state machine
    pio_sm_set_pindirs_with_mask(DATA_PIO, DATA_WRITE_SM, dir, modify);
    pio_sm_set_enabled(DATA_PIO, DATA_WRITE_SM, false);              //Don't enable state machine yet

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

void Protocol::configureDMA() {
    printf("Configuring DMA for data transfer\n");
    irq_set_exclusive_handler(DMA_IRQ_1, data_dma_done);       //Set IRQ handler for DMA transfer
    irq_set_enabled(DMA_IRQ_1, true);                          //Enable IRQ
}

void Protocol::handleProtocol() {
    if(pio_sm_get_rx_fifo_level(CMD_PIO, CMD_SM) != 0){
        //Handshake command completed (cmd lowered)
        uint8_t resp = gpioToByte(pio_sm_get_blocking(CMD_PIO, CMD_SM));
        printf("[%d] Apple response : 0x%x\n", nextState_, resp);
        if(resp != APPLE_ACK){
            setStatus(STATUS_55_NOT_RECEIVED);
            //Lower the busy line
            handshakeDone();
            setState(ProfileState::GET_COMMAND);
            return;
        }else{
            resetStatus(STATUS_55_NOT_RECEIVED);
            switch(nextState_) {
                case ProfileState::GET_COMMAND:
                    getCommand();
                    break;
                case ProfileState::READ_BLOCK:
                    readBlock();
                    break;
                case ProfileState::RCV_WRITE_DATA:
                case ProfileState::RCV_WRITE_VERIFY_DATA:
                    writeBlock();
                    break;
                case ProfileState::DO_WRITE:
                    doWrite();
                    break;
                default:
                    printf("Unsupported state : %u\n", nextState_);
                    setState(ProfileState::GET_COMMAND);
                    break;
            }
        }
    }
}


void Protocol::prepareForWrite(uint32_t count) {
    //Reset receive state machine
    pio_sm_init(DATA_PIO, DATA_WRITE_SM, pioDataWriteOffs_, &pioDataWriteCfg_);
    pio_sm_clear_fifos(DATA_PIO, DATA_WRITE_SM);
    pio_sm_set_enabled(DATA_PIO, DATA_WRITE_SM, true);

    //Prepare DMA semaphore
    sem_reset(&dataWriteSem_, 0);

    if(dataWriteDMAChan_ < 0 ){
        dataWriteDMAChan_ = dma_claim_unused_channel(true);
        dma_channel_set_irq1_enabled(dataWriteDMAChan_, true);
    }
    dma_channel_config c = dma_channel_get_default_config(dataWriteDMAChan_);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, pio_get_dreq(DATA_PIO, DATA_WRITE_SM, false));
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16); //Use 16 bits transfers are we read up to GPIO26
    dma_channel_configure(
        dataWriteDMAChan_,
        &c,
        rxBuffer_,                          // Destination pointer
        &DATA_PIO->rxf[DATA_WRITE_SM],      // Source pointer
        count,                              // Number of transfers
        true                                // Start
    );
}

void Protocol::prepareForRead(uint32_t count) {
    //Reset send state machine
    pio_sm_init(DATA_PIO, DATA_READ_SM, pioDataReadOffs_, &pioDataReadCfg_);
    pio_sm_clear_fifos(DATA_PIO, DATA_READ_SM);
    pio_sm_set_enabled(DATA_PIO, DATA_READ_SM, true);

    //Prepare DMA semaphore
    sem_reset(&dataReadSem_, 0);
    if(dataReadDMAChan_<0){
        dataReadDMAChan_ = dma_claim_unused_channel(true);
        dma_channel_set_irq1_enabled(dataReadDMAChan_, true);
    }
    dma_channel_config c = dma_channel_get_default_config(dataReadDMAChan_);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(DATA_PIO, DATA_READ_SM, true));
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    dma_channel_configure(
        dataReadDMAChan_,
        &c,
        &DATA_PIO->txf[DATA_READ_SM],   // Destination pointer
        txBuffer_,                      // Source pointer
        count,                          // Number of transfers
        true                            // Start immediately
    );
}

void Protocol::abortTransfer() {
    dma_channel_abort(dataReadDMAChan_);
    dma_channel_abort(dataWriteDMAChan_);
    pio_sm_set_enabled(DATA_PIO, DATA_READ_SM, false);
    pio_sm_set_enabled(DATA_PIO, DATA_WRITE_SM, false);
}

void Protocol::updateSpareTable() {
    uint32_t blockCount = 0;
    if(!file_->getDataBlockCount(blockCount)){
        printf("Unable to get block count from file!\n");
    }else{
        printf("Disk has %lu blocks.\n", blockCount);
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

void Protocol::getCommand() {
    printf("Getting command\n");
    for(int i=0;i<6;++i) rxBuffer_[i] = 0x0;
    prepareForWrite(6);                         //Prepare a 6 bytes transfer
    handshakeDone();                            //Lower busy
    //Wait for data bytes to be transfered (within 100ms)
    if(!sem_acquire_timeout_ms(&dataWriteSem_, 100)){
        printf("Command bytes receive timeout!\n");
        setState(ProfileState::GET_COMMAND);
    }else{
        //Build the command 
        lastCmd_.command = static_cast<ProfileCommand>(RX_TO_BYTE(0));
        lastCmd_.blockNumber = RX_TO_BYTE(1) << 16 | RX_TO_BYTE(2) << 8 | RX_TO_BYTE(3);
        lastCmd_.retryCount = RX_TO_BYTE(4);
        lastCmd_.sparesThreshold = RX_TO_BYTE(5);        
        switch(lastCmd_.command){
            case ProfileCommand::READ:
                setState(ProfileState::READ_BLOCK);
                break;
            case ProfileCommand::WRITE:
                setState(ProfileState::RCV_WRITE_DATA);
                break;
            case ProfileCommand::WRITE_VERIFY:
                setState(ProfileState::RCV_WRITE_VERIFY_DATA);
                break;
            default:
                printf("Unknown command : %x\n", nextState_);
                setState(ProfileState::GET_COMMAND);
                break;
        }
        printCommand(lastCmd_);
    }
}

void Protocol::readBlock() {
    printf("Read block\n");
    if (lastCmd_.blockNumber<0x00f00000)   lastCmd_.blockNumber = deinterleave5(lastCmd_.blockNumber);
    if(lastCmd_.blockNumber == SPARE_TABLE_ADDR){
        //Spare table, 
        memcpy(&txBuffer_[4], &spareTable_.name[0], sizeof(SpareTable));
        status_ = 0;
    }else if(lastCmd_.blockNumber == RAM_BUFFER_ADDR){
        //Copy from rx to tx
        for(int i=0;i<512+20;++i){
            txBuffer_[i+4] = RX_TO_BYTE(rxBuffer_[i]);
        }
        status_ = 0;
    }else{
        //Read to buffer + 4 to keep space for status bytes
        //TODO: Seems to start with tags, to check
        if(!file_->readTag(lastCmd_.blockNumber, txBuffer_ + 4)) {
            setStatus(STATUS_UNSUCCESS);
        }
        if(!file_->readBlock(lastCmd_.blockNumber, txBuffer_ + 4 + 20)) {
            //Read failure, need to raise some status bits
            //TODO: If failure, does the apple read all bytes?
            setStatus(STATUS_UNSUCCESS);
        }
    }
    //Build status bytes
    txBuffer_[0] = status_ & 0xFF;
    txBuffer_[1] = (status_>>8) & 0xFF;
    txBuffer_[2] = (status_>>16) & 0xFF;
    txBuffer_[3] = (status_>>24) & 0xFF;
    prepareForRead();                                   //Prepare buffer for DMA
    handshakeDone();                                    //Lower busy
    printf("Waiting for transfer to be done...\n");
    if(!sem_acquire_timeout_ms(&dataReadSem_, 100)){
        printf("Data read timeout...\n");
    }
    printf("Read cycle done!\n");
    setState(ProfileState::GET_COMMAND);    
}

void Protocol::writeBlock() {
    printf("Write block\n");    
    prepareForWrite();
    handshakeDone();
    printf("Waiting for transfer to be done...\n");
    if(!sem_acquire_timeout_ms(&dataWriteSem_, 100)){
        printf("Data write timeout...\n");
        //TODO: Handle status
    }
    setState(ProfileState::DO_WRITE);
}

void Protocol::doWrite() {
    printf("Writing to disk, block $%lx\n", lastCmd_.blockNumber);
    if (lastCmd_.blockNumber<0x00f00000) lastCmd_.blockNumber=deinterleave5(lastCmd_.blockNumber);
    uint8_t bufByte[512];
    for(int i=0;i<20;++i){
        bufByte[i] = RX_TO_BYTE(i);
    }
    file_->writeTag(lastCmd_.blockNumber, bufByte);
    for(int i=0;i<512;++i){
        bufByte[i] = RX_TO_BYTE(i);
    }
    file_->writeBlock(lastCmd_.blockNumber, bufByte);
    txBuffer_[0] = status_ & 0xFF;
    txBuffer_[1] = (status_>>8) & 0xFF;
    txBuffer_[2] = (status_>>16) & 0xFF;
    txBuffer_[3] = (status_>>24) & 0xFF;
    prepareForRead(4);
    handshakeDone();
    //Send status bytes
    if(!sem_acquire_timeout_ms(&dataReadSem_, 200)){
        printf("Write status bytes read timeout!\n");
    }
    printf("Do write completed!\n");
    setState(ProfileState::GET_COMMAND);
}

void Protocol::handshakeDone() {
    //The handshake state machine will loop until
    //the X scratch register is not 0
    pio_sm_exec(CMD_PIO, CMD_SM, pio_encode_set(pio_x, 1));
}

void Protocol::dataDMADone() {
    if(dma_channel_get_irq1_status(dataReadDMAChan_)) {
        dma_channel_acknowledge_irq1(dataReadDMAChan_);
        sem_release(&dataReadSem_);
    }
    if(dma_channel_get_irq1_status(dataWriteDMAChan_)) {
        dma_channel_acknowledge_irq1(dataWriteDMAChan_);
        sem_release(&dataWriteSem_);
    }
}

void Protocol::commandReceived() {
    pio_interrupt_clear(CMD_PIO, 0);
    //Cancel all pending DMA transfers
    abortTransfer();
}

void Protocol::setStatus(uint32_t bits) {
    status_ |= bits;
}

void Protocol::resetStatus(uint32_t bits) {
    status_ &= ~bits;
}

void Protocol::setState(ProfileState newState) {
    if(newState == ProfileState::GET_COMMAND){
        pio_sm_init(CMD_PIO, CMD_SM, pioCmdOffs_, &pioCmdCfg_);
        pio_sm_clear_fifos(CMD_PIO, CMD_SM);
        pio_sm_set_enabled(CMD_PIO, CMD_SM, true);
    }
    pio_sm_put_blocking(CMD_PIO, CMD_SM, newState);
    nextState_ = newState;
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