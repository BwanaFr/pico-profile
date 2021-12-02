#include "configFile.hxx"
#include "DC42File.hxx"
#include <cstring>
#include <cstdlib>

ConfigEntry<char*> ConfigFile::imageFileName_(DISK_IMG_SECTION, DISK_IMG_NAME, "lisaem-profile.dc42");
ConfigEntry<bool> ConfigFile::displayFileInfo_(DISPLAY_SECTION, DISPLAY_FILE_INFO, true, &imageFileName_);
ConfigEntry<bool> ConfigFile::displayImageInfo_(DISPLAY_SECTION, DISPLAY_IMAGE_INFO, true, &displayFileInfo_);
ConfigEntry<int> ConfigFile::displayStandby_(DISPLAY_SECTION, DISPLAY_STANDBY, 5000, &displayImageInfo_);

bool ConfigFile::loadFile()
{
    DC42File::mountFatFs();
    FIL file;
    FRESULT res = f_open(&file, CONFIG_FILE_NAME, FA_READ);
    if(res != FR_OK){
        return false;
    }
    char* section = nullptr;    
    while(true){
        LineValue line = getNextLine(&file);
        switch(line.type){
            case LineType::COMMENT:
                //Ignore comments
                free(line.value);
                break;
            case LineType::SECTION:
                free(section);
                section = line.key;
                break;
            case LineType::VALUE:
                if(section != nullptr){
                    AbstractConfigEntry* entry = &imageFileName_;
                    while(entry != nullptr){
                        entry->setValue(section, line.key, line.value);
                        entry = entry->getNext();
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
    free(section);
    f_close(&file);
    printf("Disk image name : %s\n", imageFileName_.getValue());
    printf("Display file info : %s\n", (displayFileInfo_.getValue() ? "YES" : "NO"));
    printf("Display image info : %s\n", (displayImageInfo_.getValue() ? "YES" : "NO"));
    printf("Display standby timeout : %d\n", displayStandby_.getValue());
    return true;
}

const char* ConfigFile::getImageFileName()
{
    return imageFileName_.getValue();
}

bool ConfigFile::displayFileName()
{
    return displayFileInfo_.getValue();
}

bool ConfigFile::displayImageInfo()
{
    return displayImageInfo_.getValue();
}

int ConfigFile::displayStandby()
{
    return displayStandby_.getValue();
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
                //Found end of section
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

AbstractConfigEntry::AbstractConfigEntry(const char* section, const char* key, AbstractConfigEntry* prev) :
        section_(section), key_(key), set_(false), next_(nullptr)
{
    if(prev != nullptr){
        prev->next_ = this;
    }
}

bool AbstractConfigEntry::setValue(const char* section, const char* key, const char* value)
{
    if(!set_){
        if((strcasecmp(section, section_) == 0) && 
            (strcasecmp(key, key_) == 0))
        {
            set_ = setValue(value);
            return set_;
        }
    }
    return false;
}

template<>
bool ConfigEntry<bool>::setValue(const char* value)
{
    if((strcasecmp(value, "true") == 0) || 
        (strcasecmp(value, "yes") == 0) ||
        (strcasecmp(value, "y") == 0))
    {
        value_ = true;
    }else{
        value_ = false;
    }
    return true;
}

template<>
bool ConfigEntry<int>::setValue(const char* value)
{
    char * endPtr;
    int newVal = strtoll(value, &endPtr, 10);
    if(endPtr == value){
        //Parse failed
        return false;
    }
    value_ = newVal;
    return true;
}


bool ConfigEntry<char*>::setValue(const char* value)
{
    free(value_);
    size_t len = strlen(value) + 1;
    value_ = (char*)calloc(len, sizeof(char));
    strcpy(value_, value);
    return true;
}


ConfigEntry<char*>::ConfigEntry(const char* section, const char* key, const char* defaultValue, AbstractConfigEntry* prev) :
    AbstractConfigEntry(section, key, prev), value_(nullptr), defaultValue_(defaultValue)
{    
}

const char* ConfigEntry<char*>::getValue() const
{
    if(value_){
        return value_;
    }
    return defaultValue_;
}

ConfigEntry<char*>::~ConfigEntry()
{
    if(value_) {
        free(value_);
    }
}