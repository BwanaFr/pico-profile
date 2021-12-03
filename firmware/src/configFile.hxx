#include <cstdio>
#include "ff.h"
#include <string>
#include <cstring>

class AbstractConfigEntry {
public:
    AbstractConfigEntry(const char* section, const char* key, AbstractConfigEntry* prev = nullptr);
    virtual ~AbstractConfigEntry();
    bool setValue(const std::string& section, const std::string& key, const std::string& value);
    inline bool isSet() const { return set_; };
    inline AbstractConfigEntry* getNext() const { return next_; }
    inline const char* getSection() const { return section_; }
    inline const char* getKey() const { return key_; }
    virtual std::string getValueAsString() const = 0;
private:
    virtual bool setStringValue(const std::string& value) = 0;    
    const char* section_;
    const char* key_;
    bool set_;
    AbstractConfigEntry* next_;
};

template<typename T>
class ConfigEntry : public AbstractConfigEntry {
public:
    ConfigEntry(const char* section, const char* key, T const &defaultValue, AbstractConfigEntry* prev = nullptr);

    virtual ~ConfigEntry();

    const T& getValue() const;

    void setValue(const T& newValue);

    std::string getValueAsString() const;

private:
    bool setStringValue(const std::string& value) override;
    T value_;   
};

template<typename T>
ConfigEntry<T>::~ConfigEntry(){}

template<typename T>
const T& ConfigEntry<T>::getValue() const {
    return value_;
}
template<typename T>
void ConfigEntry<T>::setValue(const T& newValue) {
    value_ = newValue;
}

template<typename T>
std::string ConfigEntry<T>::getValueAsString() const {
    return std::to_string(value_);
}

template<typename T>
ConfigEntry<T>::ConfigEntry(const char* section, const char* key, T const &defaultValue, AbstractConfigEntry* prev) :
    AbstractConfigEntry(section, key, prev), value_(defaultValue)
{
}

template<typename T> 
bool ConfigEntry<T>::setStringValue(const std::string& value)
{
    value_ = value;
    return true;
}

//Specialized templates
template<> bool ConfigEntry<bool>::setStringValue(const std::string& value);
template<> bool ConfigEntry<int>::setStringValue(const std::string& value);
template<> std::string ConfigEntry<bool>::getValueAsString() const;
template<> std::string ConfigEntry<std::string>::getValueAsString() const;

class ConfigFile {

public:
    /**
     * Loads the configuration file
     * @return True if file loads properly
     */
    static bool loadFile();

    /**
     * Save the configuration file with actual values
     * @return True if file is saved properly
     */
    static bool saveFile();

    /**
     * Gets the image file name to use
     * @return Image file name on SD card
     */
    static const char* getImageFileName();

    /**
     * Gets if the file name
     * must be shown on the OLED screen
     * @return True if the file name must be displayed
     */
    static bool isDisplayFileName();

    /**
     * Gets if the disk image information
     * must be shown on OLED screen
     * @return True if image info must be shown
     */
    static bool isDisplayImageInfo();

    /**
     * Gets the display standby timeout
     * @return Display standby timeout in ms
     */
    static int getDisplayStandby();

    /**
     * Gets the display off timeout
     * @return Display off timeout in ms
     **/
    static int getDisplayAutoOff();

    /**
     * Compares two string without taking into account case
     * @return true if string are identical
     */
    static inline bool strCompare(const std::string& str1, const std::string& str2){
        return ((str1.size() == str2.size()) && std::equal(str1.begin(), str1.end(), str2.begin(),
         [](const char & c1, const char & c2){
            return (c1 == c2 || std::toupper(c1) == std::toupper(c2));
            }));
    }

private:
    enum LineType {
        END_FILE,
        COMMENT,
        SECTION,
        VALUE,
    };

    typedef struct LineValue {
        LineType type;
        std::string key;
        std::string value;
        inline LineValue() : type(LineType::END_FILE){}
    }LineValue;

    /**
     * Read a complete line
     */
    static bool readLine(FIL* file, std::string& line);

    /**
     * Trims the string by removing all spaces and non-printable
     * caracters at start and end of the string
     * @param str Pointer to the string to be trimmed
     */
    static std::string& trim(std::string& str);

    static void getNextLine(FIL* file, LineValue& line);

    /**
     * Dumps the configuration to serial
     **/
    static void dumpConfiguration();

    
    static constexpr char CONFIG_FILE_NAME[] = "pp_config.ini";     //!< Configuration file name
    static constexpr char DISPLAY_SECTION[] = "Display";            //!< Display settings section
    static constexpr char DISPLAY_FILE_INFO[] = "showFileName";     //!< Show file name parameter key
    static constexpr char DISPLAY_IMAGE_INFO[] = "showImageInfo";   //!< Show image info parameter key
    static constexpr char DISPLAY_STANDBY[] = "standby";            //!< Standby timeout
    static constexpr char DISPLAY_AUTOOFF[] = "autooff";            //!< Off timeout

    static constexpr char DISK_IMG_SECTION[] = "Image";             //!< Disk image section
    static constexpr char DISK_IMG_NAME[] = "fileName";             //!< Disk image file name parameter key

    static ConfigEntry<std::string> imageFileName_;       //!< Image file name
    static ConfigEntry<bool> displayFileInfo_;      //!< Displays file information on screen
    static ConfigEntry<bool> displayImageInfo_;     //!< Displays image information on screen
    static ConfigEntry<int> displayStandby_;        //!< Display standby timeout
    static ConfigEntry<int> displayAutoOff_;        //!< Display auto-off timeout
};