#ifndef DIGIPOTDRIVER_H
#define DIGIPOTDRIVER_H

#include "hardware/gpio.h"
#include "pico/stdlib.h"

// digiPot
#define UD 10
#define INC 11
#define CS 12

class digiPotDriver
{
private:
    int cntr = 100;
    int queue = 0;
    void increment();
    void decrement();
public:
    digiPotDriver();
    void incQueue(){queue ++;}
    void decQueue(){queue --;}
    void updatePot();
    int retCntr(){return cntr;}
};

#endif // DIGIPOTDRIVER_H
