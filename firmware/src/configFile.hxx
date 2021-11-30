#include <cstdio>
#include "ff.h"

class ConfigFile {

public:
    static bool loadFile();
    static inline const char* getImageFileName() { return imageFileName_; }
    static inline bool displayFileName() { return displayFileInfo_; }
    static inline bool displayImageInfo() { return displayImageInfo_; }
    static inline int displayStandby() { return displayStandby_; }
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

    static char imageFileName_[256];    //!< Name of the hdd image
    static bool displayFileInfo_;       //!< Displays file information on screen
    static bool displayImageInfo_;      //!< Displays image information on screen
    static int displayStandby_;         //!< Display standby timeout
};