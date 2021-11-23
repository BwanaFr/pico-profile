#include "hmi.hxx"
#include "display.hxx"
#include "../protocol.hxx"
#include <cstdio>
#include <cstring>
#include <pico/multicore.h>

char* HMI::fileName_ = nullptr;
char* HMI::fileImageName_ = nullptr;
HMI::SecondLineState HMI::secLineState_ = SecondLineState::NoDisplay;
absolute_time_t HMI::secLineTimeout_ = 0;
queue_t HMI::msgQueue_;

void HMI::initialize()
{
    queue_init(&msgQueue_, sizeof(Message), 5);
    sleep_ms(2000); //Wait for the display to be ready
    Display::initialize();
    Display::showLogo();
}

void HMI::handleHMI()
{
    Message msg;
    if(queue_try_remove(&msgQueue_, &msg)){
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
        default:
            printf("Unsupported HMI update!\n");
            break;
        }
    }
    bool line1Scrolled = false;
    bool line2Scrolled = false;
    Display::animateDisplay(line1Scrolled, line2Scrolled);
    if(line2Scrolled && (absolute_time_diff_us(secLineTimeout_, get_absolute_time()) > 0)){
        switch(secLineState_){
            case SecondLineState::NoDisplay:
            case SecondLineState::DisplayFileName:
                secLineState_ = SecondLineState::DisplayFileImageName;
                if(fileName_ ){
                    char str[strlen(fileName_) + 13];
                    sprintf(str, "File name : %s", fileName_);
                    Display::setText(str, 1);
                    break;
                }
            case SecondLineState::DisplayFileImageName:
                secLineState_ = SecondLineState::DisplayFileName;
                if(fileImageName_ ){
                    char str[strlen(fileImageName_) + 14];
                    sprintf(str, "Disk image : %s", fileImageName_);
                    Display::setText(str, 1);
                    break;
                }
            default:
                secLineState_ = SecondLineState::NoDisplay;
                break;
        }
        if(secLineState_ != SecondLineState::NoDisplay){
            secLineTimeout_ = make_timeout_time_ms(2000);
        }else{
            secLineTimeout_ = 0;
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