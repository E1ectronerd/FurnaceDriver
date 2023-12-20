// C libs
#include <stdio.h>
#include <string.h>
#include <array>
using std::fill_n;

// Pi Libs
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// Project Libs
#include "furnaceDriver.h"
#include "SSD1309.h"
#include "digipotdriver.h"

#define RUN_SW 8
#define PLL_ENABLE 9
#define POT_SW 13
#define POW_LED 1

// Rotary defs
#define outputA 14
#define outputB 15
#define SW 13
#define LED 25

//static LCD2004Driver lcdDriver;

static SSD1309 oledDriver;

static digiPotDriver digPotDriver;

// fosc = M1(VCOin/R1)/2*C1*Vramp
// VCOin = 7.5
// M1 6.2
// Vramp = 2.1
// fosc = 6.2(7.5/R1)/(2*C1*2.1)

float C1 = .000000047;
float fosc(float R1)
{
    return (6.2 * (7.5/R1)) / (C1*4.2);
}

static int cntNum = 0;

// Run switch globals
static bool RUN_SW_flag = 0;
static bool RUN_SW_update = 0;

// Rotary encoder globals
static bool POT_SW_flag = 0;
static bool POT_SW_update = 0;
static int POT_SW_cntr = 0;

static bool PLL_ENABLE_flag = 0;
static bool PLL_ENABLE_update = 0;

// Timer globals
static int rTimerCntr = 0;
static int timeOn = 10;
static int timerCycleLen = 1000;

#define PwrMode 2
#define PotMode 1


//
//
// Only one sw callback per core allowed
void SW_callback(uint gpio, uint32_t events)
{
    // disable callback while callback runs
    //gpio_set_irq_enabled_with_callback (POT_SW, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, 0, &SW_callback);
    gpio_set_irq_enabled_with_callback (POT_SW, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_LEVEL_LOW, 0, &SW_callback);
    gpio_set_irq_enabled_with_callback (RUN_SW, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, 0, &SW_callback);

    //if ((gpio == POT_SW) && (events == (GPIO_IRQ_EDGE_FALL | GPIO_IRQ_LEVEL_LOW)))
    if (gpio == POT_SW)
    {
        busy_wait_ms(90);
        if (POT_SW_update == 0)
        {
            if (POT_SW_cntr >= 2)
            {
                POT_SW_cntr = 0;
                POT_SW_update = 1;
            }
            else
            {
                POT_SW_cntr ++;
                POT_SW_update = 1;
            }
        }
    }

    if (gpio == RUN_SW)
    {
        if ((RUN_SW_flag == 0) && (events == GPIO_IRQ_LEVEL_LOW))
        {
            RUN_SW_flag = 1;
            //gpio_put(POW_LED, 1);
            RUN_SW_update = 1;
            gpio_set_irq_enabled_with_callback (RUN_SW, GPIO_IRQ_LEVEL_HIGH, 1, &SW_callback);
        }

        if ((RUN_SW_flag == 1) && (events == GPIO_IRQ_LEVEL_HIGH))
        {
            RUN_SW_flag = 0;
            //gpio_put(POW_LED, 0);
            RUN_SW_update = 1;
            gpio_set_irq_enabled_with_callback (RUN_SW, GPIO_IRQ_LEVEL_HIGH, 0, &SW_callback);
            gpio_set_irq_enabled_with_callback (RUN_SW, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE | GPIO_IRQ_LEVEL_LOW, 1, &SW_callback);
        }
    }
    // enable callback
    gpio_set_irq_enabled_with_callback (POT_SW, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_LEVEL_LOW, 1, &SW_callback);
    //gpio_set_irq_enabled_with_callback (POT_SW, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, 1, &SW_callback);
    gpio_set_irq_enabled_with_callback (RUN_SW, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, 1, &SW_callback);
}


//
//
//
bool RUN_Timer_callback(struct repeating_timer *rt)
{
    if(RUN_SW_flag && (timeOn >= 15))
    {
        rTimerCntr ++;
        if ((rTimerCntr < timeOn) || (timeOn > 985))
        {
        // set PPL low to enable I think
            PLL_ENABLE_flag = 0;
            gpio_put(PLL_ENABLE, PLL_ENABLE_flag);
            PLL_ENABLE_update = 1;
            gpio_put(POW_LED, 1);
        }
        else
        {
            PLL_ENABLE_flag = 1;
            gpio_put(PLL_ENABLE, PLL_ENABLE_flag);
            PLL_ENABLE_update = 1;
            gpio_put(POW_LED, 0);
        }
        if(rTimerCntr >= timerCycleLen)
        {
            rTimerCntr = 0;
        }
    }
    else
    {
        PLL_ENABLE_flag = 1;
        gpio_put(PLL_ENABLE, PLL_ENABLE_flag);
    }
    if(!RUN_SW_flag)
    {
        PLL_ENABLE_flag = 1;
        gpio_put(PLL_ENABLE, PLL_ENABLE_flag);
        PLL_ENABLE_update = 1;
        gpio_put(POW_LED, 0);
    }
    return true;
}

//
// Core1 Processes
//
void core1_entry() {
    gpio_init(PLL_ENABLE);
    gpio_set_dir(PLL_ENABLE, GPIO_OUT);
    gpio_pull_up(PLL_ENABLE);
    gpio_put(PLL_ENABLE, 1);

    //gpio_init(POW_LED);
    //gpio_set_dir(POW_LED, GPIO_OUT);
    //gpio_put(POW_LED, 0);

    //struct repeating_timer timer;
    //add_repeating_timer_ms(	-1, RUN_Timer_callback, nullptr, &timer);

    oledDriver.init();

    oledDriver.writeLine("                  ",18, 4);
    oledDriver.writeLine("RUN SW OFF",10, 4);

    oledDriver.writeLine("                  ",18, 1);
    oledDriver.writeLine("Standby Mode",12, 1);

    while (1)
    {
        if (POT_SW_update)
        {
            asm volatile("nop \n nop \n nop");
            asm volatile("nop \n nop \n nop");

            if (int(POT_SW_cntr) == PotMode)
            {
                oledDriver.writeLine("                  ",18, 1);
                oledDriver.writeLine("Set Frequency",13, 1);
                POT_SW_update = 0;

                //digPotDriver.updatePot();
                //char potCnt[3] = "";
                //sprintf(potCnt, "%d", digPotDriver.retCntr());
                //oledDriver.writeLine("                  ",18, 2);
                //oledDriver.writeLine(potCnt,3, 2);

                //float R1 = digPotDriver.retCntr() * 101;
                //float foscCal = fosc(R1);
                //char freq[18] = "";
                //sprintf(freq, "%d", int(foscCal));
                //oledDriver.writeLine("                  ",18, 3);
                //oledDriver.writeLine(freq,18, 3);
            }
            if (int(POT_SW_cntr) == PwrMode)
            {
                oledDriver.writeLine("                  ",18, 1);

                char lnConst[12] = "On Time ms:";
                char pwrCnt[5] = "";
                sprintf(pwrCnt, "%d", timeOn);
                strcat(lnConst, pwrCnt);
                oledDriver.writeLine(lnConst,17, 1);
                POT_SW_update = 0;
            }
            if (int(POT_SW_cntr) == 0)
            {
                oledDriver.writeLine("                  ",18, 1);
                oledDriver.writeLine("Standby Mode",12, 1);
                POT_SW_update = 0;
            }
        }

        if (RUN_SW_update)
        {
            if(RUN_SW_flag)
            {
                RUN_SW_update = 0;
                oledDriver.writeLine("                  ",18, 4);
                oledDriver.writeLine("RUN SW ON",9, 4);
                //gpio_put(POW_LED, 1);

            }
            else
            {
                RUN_SW_update = 0;
                oledDriver.writeLine("                  ",18, 4);
                oledDriver.writeLine("RUN SW OFF",10, 4);
                //gpio_put(POW_LED, 0);
            }
        }
        // toggle power pin and LED
     /*
        if (PLL_ENABLE_update)
        {
            gpio_put(POW_LED, 1);
            PLL_ENABLE_update = 0;
        }
        */


    }
}


//
// Core0 Processes
//
int main()
{
    // Set up Power LED
    // Init led before secondary core
    gpio_init(POW_LED);
    gpio_set_dir(POW_LED, GPIO_OUT);
    gpio_put(POW_LED, 0);

    sleep_ms(1);
    multicore_launch_core1(core1_entry);

    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);

    gpio_init(outputA);
    gpio_init(outputB);
    gpio_set_dir(outputA, GPIO_IN);
    gpio_set_dir(outputB, GPIO_IN);

    // Set up SW
    gpio_init(POT_SW);
    gpio_set_dir(POT_SW, GPIO_IN);
    gpio_pull_up(POT_SW);

    //gpio_set_irq_enabled_with_callback (POT_SW, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE | GPIO_IRQ_LEVEL_LOW, 1, &SW_callback);
    gpio_set_irq_enabled_with_callback (POT_SW, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_LEVEL_LOW, 1, &SW_callback);

    // Set up run switch
    gpio_init(RUN_SW);
    gpio_set_dir(RUN_SW, GPIO_IN);
    gpio_pull_up(RUN_SW);

    gpio_set_irq_enabled_with_callback (RUN_SW, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE | GPIO_IRQ_LEVEL_LOW, 1, &SW_callback);

    struct repeating_timer timer;
    add_repeating_timer_ms(	-1, RUN_Timer_callback, nullptr, &timer);

    //lcdDriver.writeTest();

    //oledDriver.init();
    //oledDriver.writeLine("1234",4, 1);

    int counter = 0;
    int currentStateA;
    int lastStateA;
    int timeCntr = 0;

    while (1) {
        // Read the current state of outputA
        currentStateA = gpio_get(outputA);

        gpio_put(LED, currentStateA);

        // If last and current state of outputA are different, then pulse occurred
        // React to only 1 state change to avoid double count
        if (currentStateA != lastStateA  && currentStateA == 1){


          // If the outputB state is different than the outputA state then
          // the encoder is rotating CCW so decrement

          if (gpio_get(outputB) != currentStateA) {
            cntNum ++;
            POT_SW_update = 1;
            if (POT_SW_cntr == PotMode){
                digPotDriver.incQueue();
            }
            if ((POT_SW_cntr == PwrMode) && (timeOn <= 990)){
                timeOn += 10;
            }
          }
          else {
            // Encoder is rotating CW so increment
            cntNum --;
            POT_SW_update = 1;
            if (POT_SW_cntr == PotMode){
                digPotDriver.decQueue();
            }
            if ((POT_SW_cntr == PwrMode) && (timeOn >= 9)){
                timeOn -= 10;
            }
          }
        }
          // Remember last outputA state
          lastStateA = currentStateA;

          // Put in a slight delay to help debounce the reading
          sleep_ms(1);

        }
    return 0;
}
