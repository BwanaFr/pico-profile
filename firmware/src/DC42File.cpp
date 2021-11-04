#include "DC42File.hxx"
#include <stdio.h>
#include "pico/stdlib.h"

extern "C"{
    #include "ff.h"
    #include "sd_card.h"
}

#define CALL_CHECK(call) \
if ((!call)) { \
return false; \
}

#define CALL_FS_CHECK(call) \
if ((fatFsResult_ = call) != FR_OK) { \
return false; \
}

#define CHECK_STATUS \
if((internalError_ == NOT_INITIALIZED) || \
    (internalError_ == FILE_OPEN_ERROR) || \
    (internalError_ == WRONG_MAGIC)) \
return false; 

bool DC42File::initialized_ = false;
FATFS DC42File::fatFs_;

DC42File::DC42File() :
        fatFsResult_(FR_OK), internalError_(NOT_INITIALIZED),
        tagOffset_(0) {
}

DC42File::~DC42File() {
    closeFile();
}

bool DC42File::open(const char* file) {
    if(!initialized_) {
        CALL_CHECK(sd_init_driver());
        initialized_ = true;
        //Initialize the FatFs work area
        f_mount(&fatFs_, "", 1);
    }else{
        closeFile();
    }
    //File is not yet initialized.
    if(internalError_ == NOT_INITIALIZED) {
        fatFsResult_ = f_open(&file_, file, FA_READ | FA_WRITE);
        if(fatFsResult_ != FR_OK){
            internalError_ = FILE_OPEN_ERROR;
            return false;
        }
        //Open ok. Get Magic number
        uint16_t magic = 0;        
        if(!read(MAGIC_NUMBER, &magic)){
            internalError_ = FILE_OPEN_ERROR;
            return false;
        }
        if(magic != MAGIC_NUMBER_VAL){
            internalError_ = WRONG_MAGIC;
            return false;
        }
        uint32_t dataSize = 0;
        if(!read(DATA_BLOCK_SIZE, &dataSize)){
            internalError_ = FILE_OPEN_ERROR;
            return false;
        }
        tagOffset_ = dataSize + IMAGE_DATA;
        //TODO: Maybe add more checks, like CRC and tag size check (must be 20 bytes)
        internalError_ = NO_ERRORS;
    }
    return true;
}

void DC42File::closeFile()
{
    if(initialized_ && (internalError_ != FILE_OPEN_ERROR)
        && (internalError_ != NOT_INITIALIZED)){
        f_close(&file_);
        internalError_ == NOT_INITIALIZED;
    }
}

bool DC42File::readBlock(uint32_t blockNumber, uint8_t* data) {
    CHECK_STATUS
    uint32_t dataOffset = blockNumber * BYTES_PER_BLOCK + IMAGE_DATA;
    if(dataOffset > tagOffset_){
        internalError_ = BAD_BLOCK_NUMBER;
    }    
    CALL_FS_CHECK(f_lseek(&file_, dataOffset));
    UINT read = 0;
    CALL_FS_CHECK(f_read(&file_, data, BYTES_PER_BLOCK, &read));
    return read == BYTES_PER_BLOCK;
}

bool DC42File::readTag(uint32_t blockNumber, uint8_t* data) {
    CHECK_STATUS
    uint32_t tagOffset = blockNumber * BYTES_PER_TAG + tagOffset_;
    CALL_FS_CHECK(f_lseek(&file_, tagOffset));
    UINT read = 0;
    CALL_FS_CHECK(f_read(&file_, data, BYTES_PER_TAG, &read));
    return read == BYTES_PER_TAG;
}

bool DC42File::writeBlock(uint32_t blockNumber, const uint8_t* data) {
    CHECK_STATUS
    uint32_t dataOffset = blockNumber * BYTES_PER_BLOCK + IMAGE_DATA;
    if(dataOffset > tagOffset_){
        internalError_ = BAD_BLOCK_NUMBER;
    }    
    CALL_FS_CHECK(f_lseek(&file_, dataOffset));
    UINT wrote = 0;
    CALL_FS_CHECK(f_write(&file_, data, BYTES_PER_BLOCK, &wrote));
    return wrote == BYTES_PER_BLOCK;
}

bool DC42File::writeTag(uint32_t blockNumber, const uint8_t* data) {
    CHECK_STATUS
    uint32_t tagOffset = blockNumber * BYTES_PER_TAG + tagOffset_;
    CALL_FS_CHECK(f_lseek(&file_, tagOffset));
    UINT wrote = 0;
    CALL_FS_CHECK(f_write(&file_, data, BYTES_PER_TAG, &wrote));
    return wrote == BYTES_PER_TAG;   
}

bool DC42File::readImageName(char* data) {
    CHECK_STATUS
    uint8_t len = 0;
    CALL_CHECK(read(IMG_NAME_LEN, &len));
    
    if(len > 63){
        len = 63;
    }
    CALL_FS_CHECK(f_lseek(&file_, IMG_NAME));
    int toRead = len + 1;
    UINT read = 0;
    CALL_FS_CHECK(f_read(&file_, data, toRead, &read));
    return read == toRead;
}

bool DC42File::getDataBlockCount(uint32_t &count) {
    CHECK_STATUS
    uint32_t dataSize = 0;
    CALL_CHECK(read(DATA_BLOCK_SIZE, &dataSize));
    count = dataSize/BYTES_PER_BLOCK;
    return true;
}

bool DC42File::computeDataChecksum(uint32_t &chksum) {
    CHECK_STATUS
    chksum = 0;
    uint32_t blocks = 0;
    CALL_CHECK(getDataBlockCount(blocks));
    uint8_t data[BYTES_PER_BLOCK];
    for(uint32_t blk=0;blk<blocks;++blk){
        CALL_CHECK(readBlock(blk, data));
        for(uint16_t i=0;i<BYTES_PER_BLOCK; i+=2){
            chksum = ror32(chksum + ((uint32_t)((data[i]<<8) | data[i+1])));
        }
    }
    return true;
}

bool DC42File::computeTagChecksum(uint32_t &chksum) {
    CHECK_STATUS
    chksum = 0;
    uint32_t blocks = 0;
    CALL_CHECK(getDataBlockCount(blocks));
    uint8_t data[BYTES_PER_TAG];
    for(uint32_t blk=1;blk<blocks;++blk){
        CALL_CHECK(readTag(blk, data));
        for(uint16_t i=0;i<BYTES_PER_TAG; i+=2){
            chksum = ror32(chksum + ((uint32_t)((data[i]<<8) | data[i+1])));
        }
    }
    return true;
}

const char* DC42File::getLastError() {
    if(internalError_ != InternalError::NO_ERRORS){
        switch(internalError_){
            case InternalError::FILE_OPEN_ERROR:
                return "File open error";
            case InternalError::NOT_INITIALIZED:
                return "Not initialized";
            case InternalError::WRONG_MAGIC:
                return "Bad magic number";
            case InternalError::BAD_BLOCK_NUMBER:
                return "Bad block number";
            default:
                return "Unknown error";
        }
    }else if(fatFsResult_ != FR_OK){
        switch(fatFsResult_){
            case FR_DISK_ERR:
                return "FR_DISK_ERR";
            case FR_INT_ERR:
                return "FR_INT_ERR";
            case FR_NOT_READY:
                return "FR_NOT_READY";
            case FR_NO_FILE:
                return "FR_NO_FILE";
            case FR_NO_PATH:
                return "FR_NO_PATH";
            case FR_INVALID_NAME:
                return "FR_INVALID_NAME";
            case FR_DENIED:
                return "FR_DENIED";
            case FR_EXIST:
                return "FR_EXIST";
            case FR_INVALID_OBJECT:
                return "FR_INVALID_OBJECT";
            case FR_WRITE_PROTECTED:
                return "FR_WRITE_PROTECTED";
            case FR_INVALID_DRIVE:
                return "FR_INVALID_DRIVE";
            case FR_NOT_ENABLED:
                return "FR_NOT_ENABLED";
            case FR_NO_FILESYSTEM:
                return "FR_NO_FILESYSTEM";
            case FR_MKFS_ABORTED:
                return "FR_MKFS_ABORTED";
            case FR_TIMEOUT:
                return "FR_TIMEOUT";
            case FR_LOCKED:
                return "FR_LOCKED";
            case FR_NOT_ENOUGH_CORE:
                return "FR_NOT_ENOUGH_CORE";
            case FR_TOO_MANY_OPEN_FILES:
                return "FR_TOO_MANY_OPEN_FILES";
            case FR_INVALID_PARAMETER:
                return "FR_INVALID_PARAMETER";
            default:
                return "FR_UNKNOWN";
        }
    }
    return "No error";
}