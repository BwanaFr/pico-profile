#include <cstdio>
#include "ff.h"

class ConfigFile {

public:
    static bool loadFile();

private:
    typedef struct KeyPair {
        char* key;
        char* value;
        inline KeyPair() : key(nullptr), value(nullptr){}
    }KeyPair;
    /**
     * Try to locate the next section and set the file pointer
     * to first line after
     * @param file Pointer to configuration file
     * @return Name of the next section or nullptr if end of file reached
     */
    static char* getNextSection(FIL* file);

    /**
     * Gets the next key/value pair in the file
     * @param file Pointer to configuration file
     * @return KeyPair struct. If key is nullptr, end of file reached
     */
    static KeyPair getNextValue(FIL* file);

    /**
     * Moves the file pointer to the next
     * line
     */
    static bool moveToNextLine(FIL* file);
    
    /**
     * Gets a string between start and end
     * This function trim spaces
     **/
    static char* getString(FIL* file, FSIZE_t start, FSIZE_t end);

    static constexpr char CONFIG_FILE_NAME[] = "pp_config.ini";   //Configuration file name

    static char imageFileName_[256];    //!< Name of the hdd image
    static bool displayFileInfo_;       //!< Displays file information on screen
    static bool displayImageInfo_;      //!< Displays image information on screen
    static int displayStandby_;         //!< Display standby timeout
};