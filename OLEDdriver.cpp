#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/pio.h"

#include "SSD1331.h"
#include "SSD1309.h"
#include "OLEDdriver.h"




int main()
{
    stdio_init_all();

    // turn on LED
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);


    //SSD1331 OLEDdriverObj = SSD1331(PIN_CS, PIN_DC, PIN_MOSI, PIN_SCK);
    SSD1309 OLEDdriverObj = SSD1309(SSD1309_MAX_WIDTH, SSD1309_MAX_HEIGHT);
    //OLEDdriverObj.init();
    OLEDdriverObj._sendCmd(SSD1309_CMD_setDisplayClockDivideRatio);    //display clock divider/oscillator frequency
    OLEDdriverObj._sendCmd(0xF0);

    OLEDdriverObj._sendCmd(SSD1309_CMD_pwrOn);
    sleep_ms(50);
    OLEDdriverObj._sendCmd(SSD1309_CMD_allPixelsOn);
    //OLEDdriverObj.clearWindow(0,0,RGB_OLED_WIDTH-1,RGB_OLED_HEIGHT-1);
    sleep_ms(500);
    OLEDdriverObj.clrRAM();
    OLEDdriverObj._sendCmd(SSD1309_CMD_followRAMcontent);

    uint8_t data[20] = {0xA7,0xA7,0xA7,0xA7,0xA7,0xA7,0xA7,0xA7,0xA7,0xA7,
                        0x7A,0x7A,0x7A,0x7A,0x7A,0x7A,0x7A,0x7A,0x7A,0x7A};

    uint8_t data2[20] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    uint8_t data3[8] =  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    uint8_t cash[10] = {0x38, 0x54, 0x50, 0x38, 0x14, 0x54, 0x54, 0x38, 0x10, 0x00};


    OLEDdriverObj.drawFrame();
    OLEDdriverObj._sendCmd(SSD1309_CMD_followRAMcontent);

    OLEDdriverObj.charTest();

    return 0;
}
