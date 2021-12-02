#include <cstdio>
#include "ff.h"

class AbstractConfigEntry {
public:
    AbstractConfigEntry(const char* section, const char* key, AbstractConfigEntry* prev = nullptr);
    virtual ~AbstractConfigEntry() = default;
    bool setValue(const char* section, const char* key, const char* value);
    inline AbstractConfigEntry* getNext(){ return next_; }
private:
    virtual bool setValue(const char* value);    
    const char* section_;
    const char* key_;
    bool set_;
    AbstractConfigEntry* next_;
};

template<typename T>
class ConfigEntry : public AbstractConfigEntry {
public:
    ConfigEntry(const char* section, const char* key, T const &defaultValue, AbstractConfigEntry* prev = nullptr);

    virtual ~ConfigEntry() = default;

    const T getValue() const;

private:
    bool setValue(const char* value);

    T value_;   
};

template<typename T>
const T ConfigEntry<T>::getValue() const {
    return value_;
}

template<typename T>
ConfigEntry<T>::ConfigEntry(const char* section, const char* key, T const &defaultValue, AbstractConfigEntry* prev) :
    AbstractConfigEntry(section, key, prev), value_(defaultValue)
{
}

//Specialized templates
template<> bool ConfigEntry<bool>::setValue(const char* value);
template<> bool ConfigEntry<int>::setValue(const char* value);

//Specialized class for string
template<>
class ConfigEntry<char*> : public AbstractConfigEntry {
public:
    ConfigEntry(const char* section, const char* key, const char* defaultValue, AbstractConfigEntry* prev = nullptr);

    virtual ~ConfigEntry();

    const char* getValue() const;

private:
    bool setValue(const char* value);

    char* value_;
    const char* defaultValue_;
};


class ConfigFile {

public:
    static bool loadFile();
    static const char* getImageFileName();
    static bool displayFileName();
    static bool displayImageInfo();
    static int displayStandby();
private:
    enum LineType {
        END_FILE,
        COMMENT,
        SECTION,
        VALUE,
    };

    typedef struct LineValue {
        LineType type;
        char* key;
        char* value;
        inline LineValue() : type(LineType::END_FILE), key(nullptr), value(nullptr){}
    }LineValue;

    /**
     * Gets a value from the configuration file
     * @param section Section to find the parameter
     * @param key Key to find the parameter
     * @return Value or nullptr if not found
     **/
    static char* getValue(FIL*file, const char* section, const char* key);

    /**
     * Read a complete line
     */
    static char* readLine(FIL* file);

    /**
     * Trims the stirng by removing all spaces and non-printable
     * caracters at start and end of the string
     * @param str Pointer to the string to be trimmed
     */
    static void trim(char* str);

    static LineValue getNextLine(FIL* file);

    /**
     * Parse a boolean value
     **/
    static bool parseBool(const char* str);

    static constexpr char CONFIG_FILE_NAME[] = "pp_config.ini";     //!< Configuration file name
    static constexpr char DISPLAY_SECTION[] = "Display";            //!< Display settings section
    static constexpr char DISPLAY_FILE_INFO[] = "showFileName";     //!< Show file name parameter key
    static constexpr char DISPLAY_IMAGE_INFO[] = "showImageInfo";   //!< Show image info parameter key
    static constexpr char DISPLAY_STANDBY[] = "standby";            //!< Standby timeout

    static constexpr char DISK_IMG_SECTION[] = "Image";             //!< Disk image section
    static constexpr char DISK_IMG_NAME[] = "fileName";             //!< Disk image file name parameter key

    static ConfigEntry<char*> imageFileName_;       //!< Image file name
    static ConfigEntry<bool> displayFileInfo_;      //!< Displays file information on screen
    static ConfigEntry<bool> displayImageInfo_;     //!< Displays image information on screen
    static ConfigEntry<int> displayStandby_;        //!< Display standby timeout
};