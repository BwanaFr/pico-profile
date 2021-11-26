#include "configFile.hxx"
#include "DC42File.hxx"
#include <cstring>
#include <cstdlib>

char ConfigFile::imageFileName_[256]; 
bool ConfigFile::displayFileInfo_ = true;
bool ConfigFile::displayImageInfo_ = true;
int ConfigFile::displayStandby_ = 10000;

bool ConfigFile::loadFile()
{
    DC42File::mountFatFs();
    FIL file;
    FRESULT res = f_open(&file, CONFIG_FILE_NAME, FA_READ);
    if(res != FR_OK){
        return false;
    }
    while(true){
        char* section = getNextSection(&file);
        if(section == nullptr)
            break;
        
        printf("Found section :|%s|\n", section);
        while(true){
            KeyPair val = getNextValue(&file);
            if(val.key != nullptr){
                printf("Value : |%s| -> |%s|\n", val.key, val.value);
                free(val.key);
                free(val.value);
            }else{
                break;
            }
        }
        free(section);
    }
    f_close(&file);
    return true;
}

char* ConfigFile::getNextSection(FIL* file)
{
    char c;
    UINT read = 0;
    FSIZE_t start = 0;
    bool foundStart = false;
    while(!f_eof(file)){
        if(f_read(file, &c, 1, &read) != FR_OK){
            return nullptr;
        }
        if(c == '['){
            start = f_tell(file);
            foundStart = true;
        }
        if((c == ']') && foundStart){
            return getString(file, start, f_tell(file)-1);
        }
    }
    return nullptr;
}

ConfigFile::KeyPair ConfigFile::getNextValue(FIL* file)
{
    KeyPair ret;
    char c;
    UINT read = 0;
    FSIZE_t sepLoc = 0;
    FSIZE_t start = 0;
    FSIZE_t posBefore = f_tell(file);
    if(moveToNextLine(file)){
        start = f_tell(file);
        while(!f_eof(file)){
            if(f_read(file, &c, 1, &read) != FR_OK){
                break;
            }
            if(c == '='){
                sepLoc = f_tell(file);
            }else if((c == '\n') || (c == '\r')){
                break;
            }
        }
    }
    if(sepLoc>0){
        FSIZE_t end = f_tell(file)-1;
        ret.key = getString(file, start, sepLoc-1);
        ret.value = getString(file, sepLoc, end);
    }else{
        //Restore were we were
        f_lseek(file, posBefore);
    }
    return ret;
}

bool ConfigFile::moveToNextLine(FIL* file)
{
    char c;
    UINT read;
    bool found = false;
    while(!f_eof(file)){
        if(f_read(file, &c, 1, &read) != FR_OK){
            break;
        }
        if((c == '\n') || (c == '\r')){
            found = true;
        }else{
            if(found){
                f_lseek(file, f_tell(file)-1);
                return true;
            }
        }
    }
    return found;
}

char* ConfigFile::getString(FIL* file, FSIZE_t start, FSIZE_t end)
{
    int len = end - start;
    char* ret = (char*)calloc(len+1, sizeof(char));
    UINT read;
    f_lseek(file, start);
    if(f_read(file, ret, len, &read) != FR_OK){
        free(ret);
        return nullptr;
    }
    int realStart = 0;
    int realStop = 0;
    for(int i=0;i<len;++i){
        if(ret[i] == ' '){
            realStart++;
        }else{
            break;
        }
    }
    for(int i=(len-1);i>=0;--i){
        if(ret[i] == ' '){
            ++realStop;
        }else{
            break;
        }
    }
    for(int i=realStart;i<len;++i){
        ret[i-realStart] = ret[i];
    }
    memset(&ret[len-realStart-realStop], 0x0, realStart);
    return ret;
}