# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/home/leviathan/Documents/git/Pico_official/pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(FurnaceDriver C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(FurnaceDriver FurnaceDriver.cpp furnaceDriver.h
                             SSD1309.cpp SSD1309.h
                             digipotdriver.h digipotdriver.cpp)

pico_set_program_name(FurnaceDriver "FurnaceDriver")
pico_set_program_version(FurnaceDriver "0.1")

pico_enable_stdio_uart(FurnaceDriver 1)
pico_enable_stdio_usb(FurnaceDriver 0)

# Add the standard library to the build
target_link_libraries(FurnaceDriver pico_stdlib)

# Add any user requested libraries
target_link_libraries(FurnaceDriver
        hardware_spi
        hardware_i2c
        hardware_pio
        hardware_timer
        hardware_watchdog
        hardware_clocks
        hardware_pwm
        hardware_gpio
        pico_multicore
        pico_stdlib
        )

pico_add_extra_outputs(FurnaceDriver)

