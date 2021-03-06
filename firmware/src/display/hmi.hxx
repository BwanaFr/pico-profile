#ifndef HMI_HXX_
#define HMI_HXX_

#include <pico/util/queue.h>
#include "../profileCommand.hxx"

class HMI{
public:

    static void initializeQueue();

    /**
     * Initializes HMI objects
     **/
    static void initialize();

    /**
     * Handes HMI
     * needs to be called when CPU have time
     **/
    static void handleHMI();

    /**
     * Profile command received by protocol
     **/
    static void profileCommandReceived(const CommandMessage& cmd);

    /**
     * Sets file information
     **/
    static void setFileInfo(const char* fileName, const char* imgName);

    /**
     * Sets an error message to be displayed
     * Error messages are cleared after a timeout
     * @param msg Error message
     **/
    static void setErrorMsg(const char* msg);

    /**
     * Sets a fatal error message to be displayed
     * Fatal errors are not cleared
     * @param msg Error message
     **/
    static void setFatalMsg(const char* msg);

private:    
    HMI() = delete;
    virtual ~HMI() = delete;
    HMI& operator=(const HMI& ) = delete;

    enum MsgType{
        None,
        ProfileCommand, //!<< Profile command received
        FileName,       //!<< File name changed
        FileImageName,  //!<< File image name read
        Error,          //!<< Error occured
        Fatal,          //!<< Fatal error 
    };

    /**
     * Defines a message to be passed into queue
     **/
    typedef struct Message {
        MsgType type;   //!< Type of the message
        void* data;     //!< Associated data
        inline Message(MsgType type, void* data) : type(type), data(data) {};
        inline Message() : type(MsgType::None), data(nullptr){};
    }Message;

    enum SecondLineState {
        NoDisplay,
        DisplayFileName,
        DisplayFileImageName,
        DisplayError,
        DisplayFatal,
    };

    static constexpr int ALTERNATE_RATE = 5000;
    static constexpr int ERROR_DISP_TIME = 20000;

    static char* fileName_;
    static char* fileImageName_;
    static char* errMsg_;
    static char* fatalMsg_;
    static SecondLineState secLineState_;
    static absolute_time_t secLineTimeout_;
    static absolute_time_t displaySaverTimeout_;
    static absolute_time_t displayAutoOffTimeout_;
    static queue_t msgQueue_;

};

#endif