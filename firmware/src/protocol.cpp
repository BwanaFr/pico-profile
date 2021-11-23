#include "protocol.hxx"
#include "DC42File.hxx"
#include <stdio.h>
#include "hardware/irq.h"
#include "hardware/dma.h"
#include <string.h>
#include "display/hmi.hxx"

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
    state_(ProfileState::IDLE),
    dataReadDMAChan_(-1), dataWriteDMAChan_(-1),
    received_(0), toSend_(0), status_(0)
{
    singleton = this;    
}

Protocol::~Protocol() {
}


void Protocol::initialize() {
    //Initialize semaphores
    sem_init(&dataWriteSem_, 0, 1); 
    sem_init(&dataReadSem_, 0, 1);
    //Initialize PIO state machines
    configurePIO();
    //Initial configuration of DMA channels
    configureDMA();
    //Updates the spare table depending on file
    updateSpareTable();
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
    irq_add_shared_handler(DMA_IRQ_1, data_dma_done, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);       //Set IRQ handler for DMA transfer
    irq_set_enabled(DMA_IRQ_1, true);                          //Enable IRQ
}

void Protocol::prepareNextState() {
    switch(state_) {
        case ProfileState::IDLE:
            //Get command
            setState(ProfileState::GET_COMMAND);
            break;
        case ProfileState::GET_COMMAND:
            //Parse command
            parseCommand();            
            break;
        case ProfileState::RCV_WRITE_DATA:
        case ProfileState::RCV_WRITE_VERIFY_DATA:
            //After write to RAM, write data to SD
            setState(ProfileState::DO_WRITE);
            break;
        default:
            break;
    }
}

void Protocol::executeCurrentState() {
    switch(state_) {
        case ProfileState::GET_COMMAND:
            for(int i=0;i<6;++i) cmdRxBuffer_[i] = 0x0;
            prepareForWrite(true);                      //Prepare a up to 6 bytes transfer
            handshakeDone();                            //Lower busy
            break;
        case ProfileState::READ_BLOCK:
            //Reads data from SD and put it to RAM
            readBlock();
            //Profile will be ready to get next command
            setState(ProfileState::IDLE);
            break;
        case ProfileState::RCV_WRITE_DATA:
        case ProfileState::RCV_WRITE_VERIFY_DATA:
            //Prepare to receive data from host
            prepareForWrite();
            handshakeDone();
            break;
        case ProfileState::DO_WRITE:
            //Write RAM to disk
            doWrite();
            //Profile is now ready to get next command
            setState(ProfileState::IDLE);
            break;
        default:
            printf("executeCurrentState : Unexpected state : %x", state_);
            handshakeDone();
            setState(ProfileState::IDLE);
            break;
    }
}

void Protocol::handleProtocol() {
    if(cmdReceived_){
        cmdReceived_ = false;
        //Cancel all pending DMA transfers
        abortTransfer();
        //Command received prepare for handling next state
        prepareNextState();
        //Reset status
        status_ = 0;
    }

    if(pio_sm_get_rx_fifo_level(CMD_PIO, CMD_SM) != 0){
        //Handshake command completed (cmd lowered)
        uint8_t resp = gpioToByte(pio_sm_get_blocking(CMD_PIO, CMD_SM));
        if(resp != APPLE_ACK){
            printf("0x55 not received (got 0x%02x)!\n", resp);
            setStatus(STATUS_55_NOT_RECEIVED);
            //Lower the busy line
            handshakeDone();
            setState(ProfileState::IDLE);
            return;
        }else{
            resetStatus(STATUS_55_NOT_RECEIVED);
            executeCurrentState();
        }
    }
}


void Protocol::prepareForWrite(bool cmdBuffer) {
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
    if(cmdBuffer){
        dma_channel_configure(
            dataWriteDMAChan_,
            &c,
            cmdRxBuffer_,                       // Destination pointer
            &DATA_PIO->rxf[DATA_WRITE_SM],      // Source pointer
            CMD_RX_BUFFER_SIZE,                 // Number of transfers
            true                                // Start
        );
    }else{        
        dma_channel_configure(
            dataWriteDMAChan_,
            &c,
            rxBuffer_,                          // Destination pointer
            &DATA_PIO->rxf[DATA_WRITE_SM],      // Source pointer
            RX_BUFFER_SIZE,                     // Number of transfers
            true                                // Start
        );
    }
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

void Protocol::parseCommand() {
    //Build the command with what is received
    lastCmd_.command = static_cast<ProfileCommand>(gpioToByte(cmdRxBuffer_[0]));
    lastCmd_.blockNumber = gpioToByte(cmdRxBuffer_[1]) << 16 | gpioToByte(cmdRxBuffer_[2]) << 8 | gpioToByte(cmdRxBuffer_[3]);
    lastCmd_.retryCount = gpioToByte(cmdRxBuffer_[4]);
    lastCmd_.sparesThreshold = gpioToByte(cmdRxBuffer_[5]);     
    HMI::profileCommandReceived(lastCmd_);   
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
            setState(ProfileState::IDLE);
            break;
    }
    //printCommand(lastCmd_);
}

void Protocol::readBlock() {
    if (lastCmd_.blockNumber<0x00f00000)   lastCmd_.blockNumber = deinterleave5(lastCmd_.blockNumber);
    if(lastCmd_.blockNumber == SPARE_TABLE_ADDR){
        //Spare table, 
        memcpy(&txBuffer_[4], &spareTable_.name[0], sizeof(SpareTable));
        status_ = 0;
    }else if(lastCmd_.blockNumber == RAM_BUFFER_ADDR){
        //Copy from rx to tx
        for(int i=0;i<512+20;++i){
            txBuffer_[i+4] = RX_TO_BYTE(i);
        }
        status_ = 0;
    }else{
        //Read to buffer + 4 to keep space for status bytes
        if(!file_->readTag(lastCmd_.blockNumber, txBuffer_ + 4)) {
            if(file_->getInternalError() == DC42File::BAD_BLOCK_NUMBER){
                setStatus(STATUS_INVALID_BLOCK_NUM);
            }else{
                setStatus(STATUS_UNSUCCESS);
            }
        }
        if(!file_->readBlock(lastCmd_.blockNumber, txBuffer_ + 4 + 20)) {
            if(file_->getInternalError() == DC42File::BAD_BLOCK_NUMBER){
                setStatus(STATUS_INVALID_BLOCK_NUM);
            }else{
                setStatus(STATUS_UNSUCCESS);
            }
        }
    }
    //Build status bytes
    txBuffer_[0] = status_ & 0xFF;
    txBuffer_[1] = (status_>>8) & 0xFF;
    txBuffer_[2] = (status_>>16) & 0xFF;
    txBuffer_[3] = (status_>>24) & 0xFF;
    prepareForRead();                                   //Prepare buffer for DMA
    handshakeDone();                                    //Lower busy
}

void Protocol::doWrite() {
    if (lastCmd_.blockNumber<0x00f00000) lastCmd_.blockNumber = deinterleave5(lastCmd_.blockNumber);
    if((lastCmd_.blockNumber != SPARE_TABLE_ADDR) &&
        (lastCmd_.blockNumber != RAM_BUFFER_ADDR)){
        //Do not write spare table or RAM to disk!            
        uint8_t bufByte[512];
        for(int i=0;i<20;++i){
            bufByte[i] = RX_TO_BYTE(i);
        }
        if(!file_->writeTag(lastCmd_.blockNumber, bufByte)){
            if(file_->getInternalError() == DC42File::BAD_BLOCK_NUMBER) {
                setStatus(STATUS_INVALID_BLOCK_NUM);
            }else{
                setStatus(STATUS_UNSUCCESS);
            }
        }
        for(int i=0;i<512;++i){
            bufByte[i] = RX_TO_BYTE(i+20);
        }
        if(!file_->writeBlock(lastCmd_.blockNumber, bufByte)){
            if(file_->getInternalError() == DC42File::BAD_BLOCK_NUMBER) {
                setStatus(STATUS_INVALID_BLOCK_NUM);
            }else{
                setStatus(STATUS_UNSUCCESS);
            }
        }
    }
    if(pio_sm_get_rx_fifo_level(DATA_PIO, DATA_WRITE_SM) != 0){
        //Data still available on PIO (despite of the 532 byte DMA transfer)
        setStatus(STATUS_TOO_MUCH_DATA);
    }
    txBuffer_[0] = status_ & 0xFF;
    txBuffer_[1] = (status_>>8) & 0xFF;
    txBuffer_[2] = (status_>>16) & 0xFF;
    txBuffer_[3] = (status_>>24) & 0xFF;
    prepareForRead(4);
    handshakeDone();
    //Send status bytes
    if(!sem_acquire_timeout_ms(&dataReadSem_, 50)){
        printf("Write status bytes read timeout!\n");
    }
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
    cmdReceived_ = true;
}

void Protocol::setStatus(uint32_t bits) {
    status_ |= bits;
}

void Protocol::resetStatus(uint32_t bits) {
    status_ &= ~bits;
}

void Protocol::setState(ProfileState newState) {
    if(newState == ProfileState::IDLE){
        //IDLE command, reset the PIO state machine
        pio_sm_init(CMD_PIO, CMD_SM, pioCmdOffs_, &pioCmdCfg_);
        pio_sm_clear_fifos(CMD_PIO, CMD_SM);
        pio_sm_set_enabled(CMD_PIO, CMD_SM, true);
    }else if(newState != state_){
        pio_sm_put_blocking(CMD_PIO, CMD_SM, newState);
    }
    state_ = newState;
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