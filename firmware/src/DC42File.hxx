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
     * @return False on success
     */
    bool readBlock(uint32_t blockNumber, uint8_t* data);

    /**
     * Reads a tag for associated block number
     * @param blockNumber Associated block number
     * @param data Pointer to data buffer (must be at least 12 bytes)
     * @return False on success
     */
    bool readTag(uint32_t blockNumber, uint8_t* data);

    /**
     * Read the image name of the DC42 file
     * @param data String buffer (must allocate 64 bytes to be sure)
     * @return False on success
     */
    bool readImageName(char* data);

    /**
     * Gets number of data blocks in the file
     * @param size Number data blocks
     * @return False on success
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
     */
    void open(const char* file);
    static FATFS fatFs_;                            //!< FatFs work area
private:

    template<typename T>
    bool read(uint32_t offset, T* data) {
        fatFsResult_ = f_lseek(&file_, offset);
        if(fatFsResult_ != FR_OK){
            return true;
        }
        UINT read;
        fatFsResult_ = f_read(&file_, data, sizeof(T), &read);
        if(fatFsResult_ != FR_OK){
            return true;
        }
        return false;
    };

    static bool initialized_;                                   //!< To know if FatFs was initialized
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
    static constexpr uint8_t MAGIC_NUMBER_VAL = 0x0001;         //!< Magic number

    FRESULT fatFsResult_;                   //!< Result of FatFs calls
    enum InternalError {
        NO_ERRORS = 0,                      //!< No error present
        NOT_INITIALIZED,                    //!< Class not initialized
        FILE_OPEN_ERROR,                    //!< File is not open
        WRONG_MAGIC,                        //!< Wrong magic number
    };
    InternalError internalError_;           //!< Internal error 
    uint32_t tagOffset_;                    //!< Tag offset in the file
    FIL file_;                              //!< Pointer to file
};

#endif