#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "pico/stdlib.h"
#include "pico-profile.pio.h"
#include "pico/sem.h"

class DC42File;

class Protocol {
public:
    Protocol(DC42File* file);
    virtual ~Protocol();
    Protocol& operator=(const Protocol& ) = delete;

    /**
     * Non-blocking function for handling protocol
     */
    void handleProtocol();

    /**
     * Push data to receive buffer
     **/
    void dataReceived();

    /**
     * Data DMA read/write done IRQ
     **/
    void dataDMADone();

    /**
     * Command line lowered
     **/
    void commandReceived();
private:
    /**
     * Command byte sent by Apple
     */
    enum ProfileCommand {
        READ = 0,               //!< Read block
        WRITE,                  //!< Write block
        WRITE_VERIFY,           //!< Write and verify block
        WRITE_SPARE,            //!< Writea and force sparing
    };

    /**
     *  Next action done by profile
     **/
    enum ProfileState {
        IDLE = 0,               //!< Initial state
        GET_COMMAND,            //!< Get command
        READ_BLOCK,             //!< Read data block
        RCV_WRITE_DATA,         //!< Receive, write data block
        RCV_WRITE_VERIFY_DATA,  //!< Receive, write/verify data block
        DO_WRITE=6,             //!< Do actual write or write/verify on disk
    };

    static constexpr uint32_t SPARE_TABLE_ADDR = 0xFFFFFF;  //!< Address of the spare table
    static constexpr uint32_t RAM_BUFFER_ADDR = 0xFFFFFE;   //!< Address of the RAM buffer content
    static constexpr uint CMD_SM = 0;                       //!< Command handshake state machine number
    static constexpr uint DATA_READ_SM = 1;                 //!< Data read state machine number
    static constexpr uint DATA_WRITE_SM = 2;                //!< Data send state machine number    
    static constexpr uint APPLE_ACK = 0x55;                 //!< Apple acknowledge
    static constexpr uint TX_BUFFER_SIZE = 512+20+4;        //!< TX buffer size (512 blocks, 20 tags, 4 status)
    static constexpr uint RX_BUFFER_SIZE = 512+20;          //!< RX buffer size (512 blocks, 20 tags)
    static constexpr uint CMD_LENGTH = 6;                   //!< Number of byte in command

    //Status bits definition
    static constexpr uint32_t STATUS_UNSUCCESS = 0x1;               //!< Status unsuccessful
    static constexpr uint32_t STATUS_TIMEOUT = 0x4;                 //!< Status timeout
    static constexpr uint32_t STATUS_CRC = 0x8;                     //!< CRC error
    static constexpr uint32_t STATUS_SEEK = 0x10;                   //!< Seek error
    static constexpr uint32_t STATUS_SPARE_UPDATED = 0x20;          //!< Host data no longer in RAM
    static constexpr uint32_t STATUS_TOO_MUCH_DATA = 0x40;          //!< >532 bytes sent
    static constexpr uint32_t STATUS_55_NOT_RECEIVED = 0x80;        //!< Profile received <> 55 to its last response
    static constexpr uint32_t STATUS_SEEK_WRONG_TRACK = 0x200;      //!< Seek error, wrong track
    static constexpr uint32_t STATUS_SPARING_OCCURED = 0x400;       //!< Sparing occured
    static constexpr uint32_t STATUS_STATUS_SECTOR_READ = 0x800;    //!< Status sector reading error
    static constexpr uint32_t STATUS_BAD_BLOCK_TABLE_FULL = 0x1000; //!< Bad block table full
    static constexpr uint32_t STATUS_SPARE_TABLE_FULL = 0x4000;     //!< Spares table overflow
    static constexpr uint32_t STATUS_SEEK_HEADER = 0x8000;          //!< Seek Error - cannot read header
    static constexpr uint32_t STATUS_INVALID_BLOCK_NUM= 0x40000;    //!< Invalid block number
    static constexpr uint32_t STATUS_PROFILE_RESET = 0x80000;       //!< Profile was reset


    //GPIO definition
    static constexpr uint32_t OD0_PIN = 0;          //DO GPIO (out)
    static constexpr uint32_t OD1_PIN = 1;          //D1 GPIO (out)
    static constexpr uint32_t OD2_PIN = 2;          //D2 GPIO (out)
    static constexpr uint32_t OD3_PIN = 3;          //D3 GPIO (out)
    static constexpr uint32_t OD4_PIN = 4;          //D4 GPIO (out)
    static constexpr uint32_t OD5_PIN = 5;          //D5 GPIO (out)
    static constexpr uint32_t OD6_PIN = 6;          //D6 GPIO (out)
    static constexpr uint32_t OD7_PIN = 7;          //D7 GPIO (out)
    static constexpr uint32_t BSY_PIN = 8;          //Busy GPIO (out)
    static constexpr uint32_t RW_PIN = 9;           //R/W GPIO (in)
    static constexpr uint32_t ID0_PIN = 16;         //D0 GPIO (in)
    static constexpr uint32_t ID1_PIN = 17;         //D1 GPIO (in)
    static constexpr uint32_t ID2_PIN = 18;         //D2 GPIO (in)
    static constexpr uint32_t ID3_PIN = 19;         //D3 GPIO (in)
    static constexpr uint32_t ID4_PIN = 20;         //D4 GPIO (in)
    static constexpr uint32_t ID5_PIN = 21;         //D5 GPIO (in)
    static constexpr uint32_t ID6_PIN = 22;         //D6 GPIO (in)
    static constexpr uint32_t ID7_PIN = 26;         //D1 GPIO (in)
    static constexpr uint32_t CMD_PIN = 27;         //CMD GPIO (in)
    static constexpr uint32_t STRB_PIN = 28;        //Strobe GPIO (in)

    /**
     * Command message received
     **/
    typedef struct CommandMessage {
        ProfileCommand command;     //!< Command
        uint32_t blockNumber;       //!< Block number to read/write
        uint8_t retryCount;         //!< Retry count
        uint8_t sparesThreshold;    //!< Spare threshold
        inline CommandMessage() : command(READ), blockNumber(0), retryCount(0), sparesThreshold(0){};
    } CommandMessage;

#pragma pack(push, 1) 
    typedef struct SpareTable {
        char name[13];                  //!< Device name, should be PROFILE
        uint8_t deviceNum[3];           //!< Device ID 0,0,0x10-10MB ProFile
        uint16_t fwVersion;             //!< Firmware version
        uint8_t numBlocks[3];           //!< Number of blocks
        uint16_t bytesPerSector;        //!< Number of bytes per sector
        uint8_t spareBlocksCount;       //!< Total number of spare blocks
        uint8_t spareBlocksAllocated;   //!< Number of spare blocks allocated
        uint8_t badBlocksAllocated;     //!< Number of bad blocks currently allocated
        uint8_t spareTable[506];        //!< Spare table the numbers of the spared blocks and the bad blocks are listed (3 bytes per block number), with a $FFFFFF at the end of each list.
        inline SpareTable() : name{'P','R','O','F','I','L','E',' ',' ',' ',' ',' ',0x0},
            deviceNum{0x0, 0x0, 0x0}, fwVersion(0x9803), numBlocks{0x0,0x0,0x0},
            bytesPerSector(0x1402), spareBlocksCount(32), spareBlocksAllocated(0), badBlocksAllocated(0)
        {
            spareTable[0] = spareTable[1] = spareTable[2] = 0xFF;
            for(int i=3;i<506;++i){
                spareTable[i] = 0x0;
            }
        };
    } SpareTable;
#pragma pack(pop)

    DC42File* file_;                        //!< Pointer to file
    ProfileState nextState_;                //!< Next state
    uint pioCmdOffs_;                       //!< Cmd handshake PIO state machine offset
    pio_sm_config pioCmdCfg_;               //!< Cmd handshake PIO configuration

    uint pioDataReadOffs_;                  //!< Data read (from profile to host) PIO state machine offset
    pio_sm_config pioDataReadCfg_;          //!< Data read  (from profile to host) PIO configuration
    int dataReadDMAChan_;                   //!< Data read DMA channel
    
    uint pioDataWriteOffs_;                 //!< Data write (from host to profile) PIO state machine offset
    pio_sm_config pioDataWriteCfg_;         //!< Data write  (from host to profile) PIO configuration
    int dataWriteDMAChan_;                  //!< Data write DMA channel

    uint8_t txBuffer_[TX_BUFFER_SIZE];      //!< Data emit buffer
    uint16_t rxBuffer_[RX_BUFFER_SIZE];     //!< Data receive buffer
    uint32_t received_;                     //!< Number of data received 
    uint32_t toSend_;                       //!< Number of data to be sent
    uint32_t status_;                       //!< 4 bytes status
    CommandMessage lastCmd_;                //!< Last received command
    SpareTable spareTable_;                 //!< Spare table

    semaphore_t dataWriteSem_;              //!< Data write semaphore
    semaphore_t dataReadSem_;               //!< Data read semaphore

    /**
     * Configures PIO resources
     **/
    void configurePIO();

    /**
     * Configures DMA channels and associated interrupts
     **/
    void configureDMA();

    /**
     * Unblock the handshake state machine
     * This function will make the handshake lower the busy line
     */
    void handshakeDone();

    /**
     * Sets status bits to true
     * @param bits Bits to be set
     **/
    void setStatus(uint32_t bits);

    /**
     * Resets status bits
     * @param bits Bits to be cleared
     **/
    void resetStatus(uint32_t bits = 0xFFFFFFFF);

    /**
     * Change actual state
     **/
    void setState(ProfileState newState);

    /**
     * Initializes the data reception
     * @param count  Number of bytes to be read
     **/
    void prepareForWrite(uint32_t count = RX_BUFFER_SIZE);

    /**
     * Initializes the data send
     * @param count Number of bytes to send
     **/
    void prepareForRead(uint32_t count = TX_BUFFER_SIZE);

    /**
     * Abort TX/RX in progress transfers
     **/
    void abortTransfer();

    /**
     * Updates the spare table data
     **/
    void updateSpareTable();

    /**
     * Gets and parse command
     **/
    void getCommand();

    /**
     * Performs read block command
     **/
    void readBlock();

    /**
     * Performs write block command
     * receives the data to buffer
     **/
    void writeBlock();

    /**
     * Write block to file
     **/
    void doWrite();

    /**
     * Debug function to print received command
     **/
    static void printCommand(const CommandMessage& msg);

    /**
     * Convert GPIO from state machines to data byte
    */
    static inline uint8_t gpioToByte(uint32_t value) {
        return (value & 0x7F) | ((value >> 3) & 0x80);
    }

    /**
     * interleaves sectors
     **/
    static inline uint32_t interleave5(uint32_t sector)
    {
        static const uint32_t offset[]={0,5,10,15,4,9,14,3,8,13,2,7,12,1,6,11,16,21,26,31,20,25,30,19,24,29,18,23,28,17,22,27};
        return offset[sector&31] + sector-(sector&31);
    }

    /**
     * deinterleaves sectors
     **/
    static inline uint32_t deinterleave5(uint32_t sector)
    {
        static const uint32_t offset[]={0,13,10,7,4,1,14,11,8,5,2,15,12,9,6,3,16,29,26,23,20,17,30,27,24,21,18,31,28,25,22,19};
        return offset[sector&31] + sector-(sector&31);
    }

};

#endif