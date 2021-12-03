cmake_minimum_required(VERSION 3.13)

# Pull in Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-profile C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise the Pico SDK
pico_sdk_init()

add_subdirectory(./no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

# Add executable. Default name is the project name, version 0.1
add_executable(pico-profile 
    src/pico-profile.cpp 
    src/io_functions.hxx
    src/io_functions.cpp
    src/configFile.hxx
    src/configFile.cpp
    src/DC42File.hxx
    src/DC42File.cpp
    src/profileCommand.hxx
    src/protocol.hxx
    src/protocol.cpp
    src/hw_config.c
    src/display/display.hxx
    src/display/display.cpp
    src/display/lisa_logo.h
    src/display/font_6x13.hxx
    src/display/hmi.hxx
    src/display/hmi.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
)

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
           ${CMAKE_CURRENT_BINARY_DIR}/_version.cpp
    COMMAND ${CMAKE_COMMAND} -P
            ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake)
message(STATUS "CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")
# Generate pio header
pico_generate_pio_header(pico-profile ${CMAKE_CURRENT_LIST_DIR}/src/pico-profile.pio)

# Add the standard library to the build
target_link_libraries(pico-profile pico_stdlib pico_multicore hardware_pio hardware_i2c)

target_compile_options(pico-profile PUBLIC -Wall -Wextra -Wno-unused-function -Wno-unused-parameter)
target_compile_definitions(pico-profile PUBLIC DEBUG N_SD_CARDS=1)

pico_set_program_name(pico-profile "pico-profile")
pico_set_program_version(pico-profile "0.1")

pico_enable_stdio_uart(pico-profile 0)
pico_enable_stdio_usb(pico-profile 1)

target_link_libraries(pico-profile
    FatFs_SPI
    hardware_clocks
    hardware_pio
)

pico_add_extra_outputs(pico-profile)

