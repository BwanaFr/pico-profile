#ifndef DC_42_FILE_H_
#define DC_42_FILE_H_

#include "pico/stdlib.h"
#include "ff.h"

class DC42File {
public:
    DC42File();
    virtual ~DC42File();
    DC42File& operator=(const DC42File& ) = delete;

    /**
     * Reads a block specified by block number
     * @param blockNumber Number of the block to read
     * @param data Pointer to data buffer (must be at least 512 bytes)
     * @return True on success
     */
    bool readBlock(uint32_t blockNumber, uint8_t* data);

    /**
     * Reads a tag for associated block number
     * @param blockNumber Associated block number
     * @param data Pointer to data buffer (must be at least 20 bytes)
     * @return True on success
     */
    bool readTag(uint32_t blockNumber, uint8_t* data);

    /**
     * Writes a data block
     * @param blockNumber Number of the block to write
     * @param data Data to be written
     * @return True on success
     **/
    bool writeBlock(uint32_t blockNumber, const uint8_t* data);

    /**
     * Write a tag for associated block number
     * @param blockNumber Associated block number
     * @param data Pointer to data buffer to be written
     * @return True on success
     */
    bool writeTag(uint32_t blockNumber, const uint8_t* data);

    /**
     * Read the image name of the DC42 file
     * @param data String buffer (must allocate 64 bytes to be sure)
     * @return True on success
     */
    bool readImageName(char* data);

    /**
     * Gets number of data blocks in the file
     * @param size Number data blocks
     * @return True on success
     */
    bool getDataBlockCount(uint32_t &count);

    /**
     * Gets last error as string
     * @return Error message
     */
    const char* getLastError();

    /**
     * Closes the file
     */
    void closeFile();

    /**
     * Opens the file
     * @param file Filename
     * @return True on success
     */
    bool open(const char* file);

    /**
     * Computes the data checksum
     * @param chksum Computed data checksum
     * @return True on sucess
     **/
    bool computeDataChecksum(uint32_t &chksum);

    /**
     * Computes the tag checksum
     * @param chksum Computed data checksum
     * @return True on sucess
     **/
    bool computeTagChecksum(uint32_t &chksum);

    enum InternalError {
        NO_ERRORS = 0,                      //!< No error present
        NOT_INITIALIZED,                    //!< Class not initialized
        FILE_OPEN_ERROR,                    //!< File is not open
        WRONG_MAGIC,                        //!< Wrong magic number
        BAD_BLOCK_NUMBER,                   //!< Bad block number
    };

    /**
     * Gets internal error
     * @return Internal error value
     **/
    inline InternalError getInternalError() { return internalError_;}

    /**
     * Mount FATFS area
     **/
    static void mountFatFs();

    static constexpr uint16_t BYTES_PER_BLOCK = 512;            //!< Assume 512 bytes per block
    static constexpr uint16_t BYTES_PER_TAG = 20;               //!< Assume 20 bytes per tag

private:    
    static inline uint32_t ror32(uint32_t data) {uint32_t carry=(data & 1)<<31; return (data>>1)|carry; }
    template<typename T>
    bool read(uint32_t offset, T* data) {
        fatFsResult_ = f_lseek(&file_, offset);
        if(fatFsResult_ != FR_OK){
            return false;
        }
        UINT read;
        T tmp;
        fatFsResult_ = f_read(&file_, &tmp, sizeof(T), &read);
        if(fatFsResult_ != FR_OK){
            return false;
        }
        *data = 0;
        for(uint16_t i=sizeof(T); i>0; --i){
            *data |= (tmp & 0xFF) << ((i-1)*8);
            tmp = tmp >> 8;
        }
        return true;
    };

    //DC42 offsets
    static constexpr uint8_t IMG_NAME_LEN = 0x0;                //!< Length of image name string
    static constexpr uint8_t IMG_NAME = 0x1;                    //!< Image name
    static constexpr uint8_t DATA_BLOCK_SIZE = 0x40;            //!< Data block size in bytes
    static constexpr uint8_t DATA_TAG_SIZE = 0x44;              //!< Data block size in bytes
    static constexpr uint8_t DATA_CHECKSUM = 0x48;              //!< Data checksum
    static constexpr uint8_t TAG_CHECKSUM = 0x4c;               //!< Tag checksum
    static constexpr uint8_t DISK_ENCODING = 0x50;              //!< Disk encoding
    static constexpr uint8_t FORMAT_BYTE = 0x51;                //!< Format byte
    static constexpr uint8_t MAGIC_NUMBER = 0x52;               //!< Private Word/Magic Number offset in file
    static constexpr uint8_t IMAGE_DATA = 0x54;                 //!< Image data
    static constexpr uint16_t MAGIC_NUMBER_VAL = 0x0100;        //!< Magic number
    
    static bool fatFsInit_;                                     //!< To know if FatFs was initialized
    static FATFS fatFs_;                    //!< FatFs work area

    bool fileOpen_;                         //!< File open 
    FRESULT fatFsResult_;                   //!< Result of FatFs calls
    InternalError internalError_;           //!< Internal error 
    uint32_t tagOffset_;                    //!< Tag offset in the file
    uint32_t blockCount_;                   //!< Number of block in the file
    FIL file_;                              //!< Pointer to file
};

#endif