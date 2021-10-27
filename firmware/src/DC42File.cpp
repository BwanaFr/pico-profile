#include "DC42File.hxx"
#include <stdio.h>
#include "pico/stdlib.h"

extern "C"{
    #include "ff.h"
    #include "sd_card.h"
}

#define CALL_CHECK(call) \
if ((call)) { \
return true; \
}

#define CALL_FS_CHECK(call) \
if ((fatFsResult_ = call) != FR_OK) { \
return true; \
}

bool DC42File::initialized_ = false;
FATFS DC42File::fatFs_;

DC42File::DC42File() :
        fatFsResult_(FR_OK), internalError_(NOT_INITIALIZED),
        tagOffset_(0) {
}

DC42File::~DC42File() {
    void closeFile();
}

void DC42File::open(const char* file) {
    if(!initialized_) {
        sd_init_driver();
        initialized_ = true;
        //Initialize the FatFs work area
        f_mount(&fatFs_, "", 1);
    }
    if(internalError_ == NOT_INITIALIZED) {
        fatFsResult_ = f_open(&file_, file, FA_READ | FA_WRITE);
        if(fatFsResult_ != FR_OK){
            internalError_ = FILE_OPEN_ERROR;
            return;
        }
        //Open ok. Get Magic number
        uint16_t magic = 0;        
        if(read(MAGIC_NUMBER, &magic)){
            internalError_ = FILE_OPEN_ERROR;
            return;
        }
        if(magic != MAGIC_NUMBER_VAL){
            internalError_ = WRONG_MAGIC;
            return;
        }
        internalError_ = NO_ERRORS;
    }
}

void DC42File::closeFile()
{
    if(internalError_ != FILE_OPEN_ERROR){
        f_close(&file_);
    }
}

bool DC42File::readBlock(uint32_t blockNumber, uint8_t* data) {
    if(internalError_ == NO_ERRORS){

    }
    return false;
}

bool DC42File::readTag(uint32_t blockNumber, uint8_t* data) {
    return false;
}

bool DC42File::readImageName(char* data) {
    if(internalError_ == NO_ERRORS){
        uint8_t len = 0;
        CALL_CHECK(read(IMG_NAME_LEN, &len));
        
        if(len > 63){
            len = 63;
        }
        CALL_FS_CHECK(f_lseek(&file_, IMG_NAME));
        int toRead = len + 1;
        if(!f_gets(data, toRead, &file_)){
            return true;
        }
        return false;
    }
    return true;
}

bool DC42File::getDataBlockCount(uint32_t &count) {
    return false;
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