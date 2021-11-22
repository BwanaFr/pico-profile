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
    };

    static constexpr int ALTERNATE_RATE = 5000;

    static char* fileName_;
    static char* fileImageName_;
    static SecondLineState secLineState_;
    static absolute_time_t secLineTimeout_;
    static queue_t msgQueue_;

};

#endif