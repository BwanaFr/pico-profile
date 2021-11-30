#include "configFile.hxx"
#include "DC42File.hxx"
#include <cstring>
#include <cstdlib>

char ConfigFile::imageFileName_[256] = "lisaem-profile.dc42"; 
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
    //Get display parameters
    char* strParam = getValue(&file, DISPLAY_SECTION, DISPLAY_FILE_INFO);
    if(strParam != nullptr) {
        displayFileInfo_ = parseBool(strParam);
        free(strParam);
    }
    strParam = getValue(&file, DISPLAY_SECTION, DISPLAY_IMAGE_INFO);
    if(strParam != nullptr) {
        displayImageInfo_ = parseBool(strParam);
        free(strParam);
    }
    strParam = getValue(&file, DISPLAY_SECTION, DISPLAY_STANDBY);
    if(strParam != nullptr) {
        char* end;
        int timeout = strtoll(strParam, &end, 10);
        if(end != strParam){
            displayStandby_ = timeout;
        }
        free(strParam);
    }
    //Get file image name parameter
    strParam = getValue(&file, DISK_IMG_SECTION, DISK_IMG_NAME);
    if(strParam != nullptr) {
        strcpy(imageFileName_, strParam);
        free(strParam);
    }
    printf("File image name : |%s|\n", imageFileName_);
    f_close(&file);
    return true;
}

char* ConfigFile::getValue(FIL* file, const char* section, const char* key)
{
    //Move file to start
    f_lseek(file, 0);
    bool sectionFound = false;
    while(true){
        LineValue line = getNextLine(file);
        switch(line.type){
            case LineType::COMMENT:
                free(line.value);
                break;
            case LineType::SECTION:
                if(strcasecmp(line.key, section) == 0){
                    sectionFound = true;
                }else{
                    sectionFound = false;
                }
                free(line.key);
                break;
            case LineType::VALUE:
                if(sectionFound){
                    if(strcasecmp(line.key, key) == 0){
                        free(line.key);
                        return line.value;
                    }
                }
                free(line.key);
                free(line.value);
            default:
                break;
        }
        if(line.type == LineType::END_FILE){
            break;
        }
    }
    return nullptr;
}

char* ConfigFile::readLine(FIL* file)
{
    char c;
    UINT read = 0;
    FSIZE_t start = f_tell(file);
    FSIZE_t end = 0;
    while(!f_eof(file)){
        if(f_read(file, &c, 1, &read) != FR_OK){
            return nullptr;
        }
        if((c == '\n') || (c == '\r')){
            break;
        }
    }
    end = f_tell(file);
    if(end == start){
        return nullptr;
    }
    int len = end - start - 1;
    char* ret = (char*)calloc(len+1, sizeof(char));
    f_lseek(file, start);
    if(f_read(file, ret, len, &read) != FR_OK){
        free(ret);
        return nullptr;
    }
    f_lseek(file, end);
    trim(ret);
    return ret;
}

void ConfigFile::trim(char* str)
{
    int len = strlen(str);
    int realStart = 0;
    int realStop = 0;
    for(int i=0;i<len;++i){
        if((str[i]<=' ') || (str[i]>'~')){
            realStart++;
        }else{
            break;
        }
    }
    for(int i=(len-1);i>=0;--i){
        if((str[i]<=' ') || (str[i]>'~')){
            ++realStop;
        }else{
            break;
        }
    }
    for(int i=realStart;i<len;++i){
        str[i-realStart] = str[i];
    }
    int toCut = len-realStart-realStop;
    memset(&str[toCut], 0x0, len-toCut);
}

ConfigFile::LineValue ConfigFile::getNextLine(FIL* file)
{
    LineValue ret;
    char* line = readLine(file);
    if(line != nullptr){
        int len = strlen(line);
        if(line[0] == '['){
            //Maybe a section
            int endSec = -1;
            for(int i=1; i<len; ++i){
                if(line[i] == ']'){
                    endSec = i;
                    break;
                }
            }
            if(endSec>0){
                //FOund end of section
                ret.type = LineType::SECTION;
                ret.key = (char*)calloc(endSec, sizeof(char));
                memcpy(ret.key, &line[1], endSec-1);
                free(line);
                trim(ret.key);
                line = nullptr;
            }
        }else if(line[0] != '#'){
            int sepLoc = -1;
            for(int i=0;i<len;++i){
                if(line[i] == '='){
                    sepLoc = i;
                    break;
                }
            }
            if(sepLoc>0){
                ret.key = (char*)calloc(sepLoc, sizeof(char));
                ret.value = (char*)calloc(len+1-sepLoc, sizeof(char));
                memcpy(ret.key, line, sepLoc);
                memcpy(ret.value, &line[sepLoc+1], len-sepLoc);
                free(line);
                trim(ret.key);
                trim(ret.value);
                ret.type = LineType::VALUE;
                line = nullptr;
            }
        }
        if(line != nullptr){
            //Default to comment
            ret.value = line;
            ret.type = LineType::COMMENT;
        }
    }
    return ret;
}

bool ConfigFile::parseBool(const char* str)
{
    if((strcasecmp(str, "true") == 0) || 
        (strcasecmp(str, "yes") == 0) ||
        (strcasecmp(str, "y") == 0))
    {
            return true;
    }   
    return false;
}