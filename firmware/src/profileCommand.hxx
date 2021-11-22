#ifndef PROFILE_COMMAND_HXX_
#define PROFILE_COMMAND_HXX_

#include <cstdint>

/**
 * Command byte sent by Apple
 **/
enum ProfileCommand {
    READ = 0,               //!< Read block
    WRITE,                  //!< Write block
    WRITE_VERIFY,           //!< Write and verify block
    WRITE_SPARE,            //!< Writea and force sparing
};

/**
 * Command message received
 **/
typedef struct CommandMessage {
    ProfileCommand command;     //!< Command
    uint32_t blockNumber;       //!< Block number to read/write
    uint8_t retryCount;         //!< Retry count
    uint8_t sparesThreshold;    //!< Spare threshold
    inline CommandMessage() : command(READ), blockNumber(0), retryCount(0), sparesThreshold(0){};
} CommandMessage;

#endif