#include "hmi.hxx"
#include <cstdio>
#include <cstring>
#include <pico/multicore.h>
#include <string>

#include "display.hxx"
#include "../protocol.hxx"
#include "../version.hxx"
#include "../configFile.hxx"

char* HMI::fileName_ = nullptr;
char* HMI::fileImageName_ = nullptr;
char* HMI::errMsg_ = nullptr;
char* HMI::fatalMsg_ = nullptr;
HMI::SecondLineState HMI::secLineState_ = SecondLineState::NoDisplay;
absolute_time_t HMI::secLineTimeout_ = 0;
absolute_time_t HMI::displaySaverTimeout_ = 0;
absolute_time_t HMI::displayAutoOffTimeout_ = 0;
queue_t HMI::msgQueue_;

void HMI::initialize()
{
    queue_init(&msgQueue_, sizeof(Message), 5);
    sleep_ms(1800); //Wait for the display to be ready
    Display::initialize();
    std::string verString = "Pico-profile ";
    verString += getGitVersion();
    Display::setText(verString.c_str(), 0, false);
    Display::showLogo();
    displaySaverTimeout_ = make_timeout_time_ms(ConfigFile::getDisplayStandby());
    displayAutoOffTimeout_ = make_timeout_time_ms(ConfigFile::getDisplayAutoOff());
}

void HMI::handleHMI()
{
    Message msg;
    bool forceRefresh = false;
    if(queue_try_remove(&msgQueue_, &msg)){
        displaySaverTimeout_ = make_timeout_time_ms(ConfigFile::getDisplayStandby());
        displayAutoOffTimeout_ = make_timeout_time_ms(ConfigFile::getDisplayAutoOff());
        Display::setDisplayOn(true);
        Display::setDisplayContrast(0xFF);
        switch(msg.type){
        case MsgType::ProfileCommand:
        {
            char operation = 'R';
            CommandMessage* cmd = static_cast<CommandMessage*>(msg.data);
            if(cmd->command != ProfileCommand::READ){
                operation = 'W';
                operation |= 0x80; //Make this reverse
            }
            char infoStr[64];
            if(cmd->blockNumber == Protocol::SPARE_TABLE_ADDR){
                sprintf(infoStr, "%c    SPARE TABLE", operation);
            }else{
                sprintf(infoStr, "%c B:$%06lx R:%u S:%u", operation, cmd->blockNumber, cmd->retryCount, cmd->sparesThreshold);
            }
            Display::setText(infoStr);
            //Free the payload
            delete cmd;
            break;
        }
        case MsgType::FileName:
        {
            char* fileName = static_cast<char*>(msg.data);
            if(fileName_)
                delete[] fileName_;
            fileName_ = new char[strlen(fileName) + 1];
            strcpy(fileName_, fileName);
            delete[] fileName;
            break;
        }
        case MsgType::FileImageName:
        {
            char* fileImageName = static_cast<char*>(msg.data);
            if(fileImageName_)
                delete[] fileImageName_;
            fileImageName_ = new char[strlen(fileImageName) + 1];
            strcpy(fileImageName_, fileImageName);
            delete[] fileImageName;
            break;
        }
        case MsgType::Error:
        {
            char* errTxt = static_cast<char*>(msg.data);
            if(errMsg_)
                delete[] errMsg_;
            errMsg_ = new char[strlen(errTxt) + 1];
            strcpy(errMsg_, errTxt);
            secLineState_ = SecondLineState::DisplayError;
            delete[] errTxt;
            break;
        }
        case MsgType::Fatal:
        {
            char* errTxt = static_cast<char*>(msg.data);
            if(fatalMsg_)
                delete[] fatalMsg_;
            fatalMsg_ = new char[strlen(errTxt) + 1];
            strcpy(fatalMsg_, errTxt);
            secLineState_ = SecondLineState::DisplayFatal;
            delete[] errTxt;
            forceRefresh = true;
            break;
        }
        default:
            printf("Unsupported HMI update!\n");
            break;
        }
    }
    //Shutdown screen if timeout occured
    if((absolute_time_diff_us(displaySaverTimeout_, get_absolute_time()) > 0) && ConfigFile::getDisplayStandby() > 0)
    {
        Display::setDisplayContrast(0);
    }
    if((absolute_time_diff_us(displayAutoOffTimeout_, get_absolute_time()) > 0) && ConfigFile::getDisplayAutoOff() > 0)
    {
        Display::setDisplayOn(false);
    }
    //Animate display (scrolling + alternate lines)
    bool line1Scrolled = false;
    bool line2Scrolled = false;
    Display::animateDisplay(line1Scrolled, line2Scrolled);
    if((line2Scrolled && (absolute_time_diff_us(secLineTimeout_, get_absolute_time()) > 0)) || 
        forceRefresh
    ){
        switch(secLineState_){
            case SecondLineState::NoDisplay:
            case SecondLineState::DisplayFileName:
                secLineState_ = SecondLineState::DisplayFileImageName;
                if(ConfigFile::isDisplayFileName())
                {
                    if(fileName_ ){
                        char str[strlen(fileName_) + 13];
                        sprintf(str, "File name : %s", fileName_);
                        Display::setText(str, 1);
                    }
                    //Show this information during 2 seconds
                    secLineTimeout_ = make_timeout_time_ms(2000);
                    break;
                }
            // fall through
            case SecondLineState::DisplayFileImageName:
                secLineState_ = SecondLineState::DisplayFileName;
                if(ConfigFile::isDisplayImageInfo())
                {
                    if(fileImageName_ ){
                        char str[strlen(fileImageName_) + 14];
                        sprintf(str, "Disk image : %s", fileImageName_);
                        Display::setText(str, 1);
                    }
                    secLineTimeout_ = make_timeout_time_ms(2000);
                }
                break;
            case SecondLineState::DisplayError:
                secLineState_ = SecondLineState::DisplayFileName;
                if(errMsg_ ){
                    char str[strlen(errMsg_) + 9];
                    sprintf(str, "Error : %s", errMsg_);
                    for(int i=0;i<7;++i){
                        str[i] |= 0x80; //Make Error: reverse
                    }
                    Display::setText(str, 1);
                }
                secLineTimeout_ = make_timeout_time_ms(5000);
                break;
            case SecondLineState::DisplayFatal:
                //Fatal will stay displayed all the time   
                if(fatalMsg_ ){
                    char str[strlen(fatalMsg_) + 9];
                    sprintf(str, "Fatal : %s", fatalMsg_);
                    for(int i=0;i<7;++i){
                        str[i] |= 0x80; //Make Fatal : reverse
                    }
                    Display::setText(str, 1);
                }
                secLineTimeout_ = make_timeout_time_ms(60000);
                break;
            default:
                secLineState_ = SecondLineState::NoDisplay;
                secLineTimeout_ = 0;
                break;
        }        
    }
}

void HMI::profileCommandReceived(const CommandMessage& cmd)
{
    CommandMessage* cmdData = new CommandMessage(cmd);
    Message msg(MsgType::ProfileCommand, cmdData);
    if(!queue_try_add(&msgQueue_, &msg)){
        delete cmdData;
    }
}

void HMI::setFileInfo(const char* fileName, const char* imgName)
{
    if(fileName) {
        size_t len = strlen(fileName);
        char* name = new char[len+1];
        strcpy(name, fileName);
        Message msg(MsgType::FileName, name);
        if(!queue_try_add(&msgQueue_, &msg)){
            delete[] name;
        }
    }
    
    if(imgName) {
        size_t len = strlen(imgName);
        char* name = new char[len+1];
        strcpy(name, imgName);
        Message msg(MsgType::FileImageName, name);
        if(!queue_try_add(&msgQueue_, &msg)){
            delete[] name;
        }
    }
}

void HMI::setErrorMsg(const char* msg)
{
    if(msg) {
        size_t len = strlen(msg);
        char* message = new char[len+1];
        strcpy(message, msg);
        Message msg(MsgType::Error, message);
        if(!queue_try_add(&msgQueue_, &msg)){
            delete[] message;
        }
    }
}

void HMI::setFatalMsg(const char* msg)
{
    if(msg) {
        size_t len = strlen(msg);
        char* message = new char[len+1];
        strcpy(message, msg);
        Message msg(MsgType::Fatal, message);
        if(!queue_try_add(&msgQueue_, &msg)){
            delete[] message;
        }
    }
}