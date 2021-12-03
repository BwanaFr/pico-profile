#include "configFile.hxx"
#include "DC42File.hxx"
#include <cstring>
#include <cstdlib>

//Init configuration parameters. Order of parent is important
//Because the saveFile() method writes values without checking if section already exists
ConfigEntry<std::string> ConfigFile::imageFileName_(DISK_IMG_SECTION, DISK_IMG_NAME, "lisaem-profile.dc42");
ConfigEntry<bool> ConfigFile::displayFileInfo_(DISPLAY_SECTION, DISPLAY_FILE_INFO, true, &imageFileName_);
ConfigEntry<bool> ConfigFile::displayImageInfo_(DISPLAY_SECTION, DISPLAY_IMAGE_INFO, true, &displayFileInfo_);
ConfigEntry<int> ConfigFile::displayStandby_(DISPLAY_SECTION, DISPLAY_STANDBY, 10, &displayImageInfo_);
ConfigEntry<int> ConfigFile::displayAutoOff_(DISPLAY_SECTION, DISPLAY_AUTOOFF, -1, &displayStandby_);

bool ConfigFile::loadFile()
{
    DC42File::mountFatFs();
    FIL file;
    FRESULT res = f_open(&file, CONFIG_FILE_NAME, FA_READ);
    if(res != FR_OK){
        return false;
    }
    std::string section;
    while(true){
        LineValue line;
        getNextLine(&file, line);
        switch(line.type){
            case LineType::COMMENT:
                //Ignore comments
                break;
            case LineType::SECTION:
                section = line.key;
                break;
            case LineType::VALUE:
                if(!section.empty()){
                    AbstractConfigEntry* entry = &imageFileName_;
                    while(entry != nullptr){
                        entry->setValue(section, line.key, line.value);
                        entry = entry->getNext();
                    }
                }
                break;
            default:
                break;
        }
        if(line.type == LineType::END_FILE){
            break;
        }
    }
    
    f_close(&file);
    dumpConfiguration();
    return true;
}

bool ConfigFile::saveFile()
{
    FIL file;
    FRESULT res = f_open(&file, CONFIG_FILE_NAME, FA_CREATE_ALWAYS | FA_WRITE);
    if(res != FR_OK){
        return false;
    }
    AbstractConfigEntry* entry = &imageFileName_;
    std::string section;
    while(entry != nullptr){
        UINT wrote;
        if(entry->isSet()){
            //Only write values different from defaults
            if(!strCompare(section, entry->getSection())){
                section = entry->getSection();
                //New section
                const char b = '[';
                f_write(&file, &b, sizeof(char), &wrote);
                f_write(&file, section.c_str(), section.length(), &wrote);
                const char* end = "]\n";
                f_write(&file, end, strlen(end), &wrote);
            }
            std::string key = entry->getKey();
            f_write(&file, key.c_str(), key.length(), &wrote);
            const char e = '=';
            f_write(&file, &e, sizeof(char), &wrote);
            std::string valStr = entry->getValueAsString();
            f_write(&file, &valStr[0], valStr.size(), &wrote);
            const char cr = '\n';
            f_write(&file, &cr, sizeof(char), &wrote);
        }
        entry = entry->getNext();
    }
    f_close(&file);
    return true;
}

void ConfigFile::dumpConfiguration()
{
    printf("===============================\n");
    printf("Configuration file : \n");
    AbstractConfigEntry* entry = &imageFileName_;
    while(entry != nullptr){
        printf("%s -> %s=%s\n", entry->getSection(), entry->getKey(),
                    entry->getValueAsString().c_str());
        entry = entry->getNext();
    }
    printf("===============================\n");
}

const char* ConfigFile::getImageFileName()
{
    return imageFileName_.getValue().c_str();
}

bool ConfigFile::isDisplayFileName()
{
    return displayFileInfo_.getValue();
}

bool ConfigFile::isDisplayImageInfo()
{
    return displayImageInfo_.getValue();
}

int ConfigFile::getDisplayStandby()
{
    return displayStandby_.getValue() * 1000;   //Configuration file stores it in seconds
}

int ConfigFile::getDisplayAutoOff()
{
    int ret = displayAutoOff_.getValue();
    if(ret == -1){
        //If auto-off is not set do twice of the dim time
        ret = displayStandby_.getValue() * 2;
        displayAutoOff_.setValue(ret);
    }
    return ret * 1000;   //Configuration file stores it in seconds
}

bool ConfigFile::readLine(FIL* file, std::string& line)
{
    char c;
    UINT read = 0;

    if(f_eof(file)){
        return false;
    }
    while(!f_eof(file)){
        if(f_read(file, &c, 1, &read) != FR_OK){
            return false;
        }
        if((c == '\n') || (c == '\r')){
            break;
        }
        line += c;
    }
    line = trim(line);
    return true;
}
std::string& ConfigFile::trim(std::string& str)
{
    const char* ws = " \t\n\r\f\v";
    str.erase(str.find_last_not_of(ws) + 1);
    str.erase(0, str.find_first_not_of(ws));
    return str;
}

void ConfigFile::getNextLine(FIL* file, LineValue& lineValue)
{
    std::string line; 
    lineValue.type = LineType::END_FILE;
    if(readLine(file, line)){
        if(line[0] == '['){
            //Maybe a section
            std::string::size_type endSec = line.find_last_of(']');
            
            if(endSec != std::string::npos){
                //Found end of section
                lineValue.type = LineType::SECTION;
                lineValue.key = line.substr(1, endSec-1);
            }
        }else if(line[0] != '#'){
            std::string::size_type sepLoc = line.find_first_of('=');
            
            if(sepLoc != std::string::npos){
                lineValue.key = line.substr(0, sepLoc);
                lineValue.value = line.substr(sepLoc+1);
                
                trim(lineValue.key);
                trim(lineValue.value);
                lineValue.type = LineType::VALUE;
            }
        }
        if(lineValue.type == LineType::END_FILE){
            //Default to comment
            lineValue.value = line;
            lineValue.type = LineType::COMMENT;
        }
    }
}

AbstractConfigEntry::AbstractConfigEntry(const char* section, const char* key, AbstractConfigEntry* prev) :
        section_(section), key_(key), set_(false), next_(nullptr)
{
    if(prev != nullptr){
        prev->next_ = this;
    }
}

AbstractConfigEntry::~AbstractConfigEntry()
{
}

bool AbstractConfigEntry::setValue(const std::string& section, const std::string& key, const std::string& value)
{
    if(!set_){
        if(ConfigFile::strCompare(section, section_) && 
            ConfigFile::strCompare(key.c_str(), key_))
        {
            set_ = setStringValue(value);
            return set_;
        }
    }
    return false;
}

template<>
bool ConfigEntry<bool>::setStringValue(const std::string& value)
{
    if((strcasecmp(value.c_str(), "true") == 0) || 
        (strcasecmp(value.c_str(), "yes") == 0) ||
        (strcasecmp(value.c_str(), "y") == 0))
    {
        value_ = true;
    }else{
        value_ = false;
    }
    return true;
}

template<>
bool ConfigEntry<int>::setStringValue(const std::string& value)
{
    char * endPtr;
    const char* start = value.c_str();
    int newVal = strtoll(start, &endPtr, 10);
    if(endPtr == start){
        //Parse failed
        return false;
    }
    value_ = newVal;
    return true;
}

template<>
std::string ConfigEntry<bool>::getValueAsString() const
{
    std::string ret = "false";
    if(value_){
        ret = "true";
    }
    return ret;
}

template<>
std::string ConfigEntry<std::string>::getValueAsString() const
{
    return value_;
}
