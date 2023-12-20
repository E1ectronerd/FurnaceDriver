#ifndef OLEDDRIVER_H
#define OLEDDRIVER_H

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MOSI 3   // SCA Main Out Subnode In
#define PIN_MISO 4   // Not used for display
#define PIN_CS   5   // Chip Select
#define PIN_SCK  2   // D0 Clock SCLK
#define PIN_DC   6   // Data/Command 1=data 0=command
#define PIN_RES  7   // Reset 0=reset 1=operate

#endif // OLEDDRIVER_H
