#include "digipotdriver.h"

digiPotDriver::digiPotDriver()
{
    gpio_init(UD);
    gpio_init(INC);
    gpio_init(CS);
    gpio_set_dir(UD, GPIO_OUT);
    gpio_set_dir(INC, GPIO_OUT);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_put(CS,0);
}


//
//
//
void digiPotDriver::increment()
{

    gpio_put(UD,1);
    gpio_put(INC,0);
    sleep_us(20);
    gpio_put(INC,1);
    sleep_us(20);
    gpio_put(INC,0);
    cntr ++;
}

//
//
//
void digiPotDriver::decrement()
{
    gpio_put(UD,0);
    gpio_put(INC,0);
    sleep_us(20);
    gpio_put(INC,1);
    sleep_us(20);
    gpio_put(INC,0);
    cntr --;

}

//
//
//
void digiPotDriver::updatePot()
{
    if ((queue + cntr) > 100)
    {
        queue -= ((queue + cntr) - 100);
    }

    if ((queue + cntr) < 1)
    {
        queue += -((queue-1) + cntr);
    }

    if ((queue > 0) && (cntr < 100))
    {
        for (int i=0; i<queue; i++)
        {
            increment();
        }
    }
    else if ((queue < 0) && (cntr > 1))
    {
        for (int i=0; i<-queue; i++)
        {
            decrement();
        }
    }
    queue = 0;
}
