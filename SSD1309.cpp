#include "SSD1309.h"
#include <math.h>
#include <map>
#include <vector>
const SSD1309_Bite_t ON = { 0x01 };
const SSD1309_Bite_t OFF = { 0x00 };

// Capital Font Array
static uint8_t capFont_8x5[27][5] =   {{0xF8,0x24,0x22,0x24,0xF8},   //A
                                       {0xFE,0x92,0x92,0x92,0x6C},   //B
                                       {0x7C,0x82,0x82,0x82,0x44},   //C
                                       {0xFE,0x82,0x82,0x82,0x7C},   //D
                                       {0xFE,0x92,0x92,0x92,0x82},   //E
                                       {0xFE,0x12,0x12,0x02,0x02},   //F
                                       {0x7C,0x82,0x92,0x92,0xE4},   //G
                                       {0xFE,0x10,0x10,0x10,0xFE},   //H
                                       {0x00,0x82,0xFE,0x82,0x00},   //I
                                       {0x40,0x82,0x82,0x7E,0x02},   //J
                                       {0xFE,0x10,0x38,0x44,0x82},   //K
                                       {0xFE,0x80,0x80,0x80,0x00},   //L
                                       {0xFE,0x04,0x08,0x04,0xFE},   //M
                                       {0xFE,0x04,0x08,0x10,0xFE},   //N
                                       {0x7C,0x82,0x82,0x82,0x7C},   //O
                                       {0xFE,0x12,0x12,0x12,0x0C},   //P
                                       {0x7C,0x82,0xA2,0x42,0xBC},   //Q
                                       {0xFE,0x12,0x12,0x32,0xCC},   //R
                                       {0x4C,0x92,0x92,0x92,0x64},   //S
                                       {0x02,0x02,0xFE,0x02,0x02},   //T
                                       {0x7E,0x80,0x80,0x80,0x7E},   //U
                                       {0x1E,0x60,0x80,0x60,0x1E},   //V
                                       {0x7E,0x80,0x70,0x80,0x7E},   //W
                                       {0xC6,0x28,0x10,0x28,0xC6},   //X
                                       {0x06,0x08,0xF0,0x08,0x06},   //Y
                                       {0xC2,0xA2,0x92,0x8A,0x86},   //Z
                                       {0x00,0x00,0x00,0x00,0x00}};  //Space


// Lower Case Font Array
static uint8_t lowFont_8x5[27][5] = {{0x60,0x94,0x94,0x94,0xF8},    //a
                                     {0xFE,0x90,0x90,0x90,0x60},    //b
                                     {0x70,0x88,0x88,0x88,0x10},    //c
                                     {0x60,0x90,0x90,0x90,0xFE},    //d
                                     {0x78,0x94,0x94,0x94,0x18},    //e
                                     {0x10,0xFC,0x12,0x12,0x00},    //f
                                     {0x18,0xA4,0xA4,0xA4,0x7C},    //g
                                     {0xFE,0x30,0x10,0x10,0xE0},    //h
                                     {0x00,0x90,0xF4,0x80,0x00},    //i
                                     {0x40,0x80,0x88,0x7A,0x08},    //j
                                     {0xFE,0x10,0x28,0xC4,0x00},    //k
                                     {0x7C,0x80,0x80,0x00,0x00},    //l
                                     {0xE0,0x10,0x60,0x10,0xE0},    //m
                                     {0xE0,0x20,0x10,0x10,0xE0},    //n
                                     {0x70,0x88,0x88,0x88,0x70},    //o
                                     {0xF8,0x48,0x48,0x48,0x30},    //p
                                     {0x30,0x48,0x48,0x48,0xF8},    //q
                                     {0x00,0xF0,0x20,0x10,0x10},    //r
                                     {0x10,0xA8,0xA8,0x40,0x00},    //s
                                     {0x08,0x7C,0x88,0x88,0x00},    //t
                                     {0x70,0x80,0x80,0x40,0xF0},    //u
                                     {0x30,0x40,0x80,0x40,0x30},    //v
                                     {0x70,0x80,0x60,0x80,0x70},    //w
                                     {0x88,0x50,0x20,0x50,0x88},    //x
                                     {0x98,0xA0,0x60,0x18,0x00},    //y
                                     {0xC8,0xA8,0x98,0x88,0x00},    //z
                                     {0x00,0x00,0x00,0x00,0x00},};  //space
// Index Lookup Array
//static char lowFont_8x5_index[27] = {};


// Number Font Array
static uint8_t numFont_8x5[10][5] = {{0x7C,0x82,0xBA,0x82,0x7C},    //0
                                     {0x84,0x84,0xFE,0x80,0x80},    //1
                                     {0x84,0xC2,0xA2,0x92,0x8C},    //2
                                     {0x82,0x92,0x92,0x92,0x6C},    //3
                                     {0x30,0x28,0x24,0xFE,0x20},    //4
                                     {0x4E,0x8A,0x8A,0x8A,0x72},    //5
                                     {0x7C,0x92,0x92,0x92,0x64},    //6
                                     {0x06,0xC2,0x22,0x12,0x0E},    //7
                                     {0x6C,0x92,0x92,0x92,0x6C},    //8
                                     {0x0C,0x92,0x92,0x52,0x3C}};   //9

struct oledChar
{

};


// Number Lookup Map
/*
const std::map<char, uint8_t(*)[5]> numMap{{'0',{0x7C,0x82,0xBA,0x82,0x7C}},
                                   {'1',{0x84,0x84,0xFE,0x80,0x80}},
                                   {'2',{0x84,0xC2,0xA2,0x92,0x8C}},
                                   {'3',{0x82,0x92,0x92,0x92,0x6C}},
                                   {'4',{0x30,0x28,0x24,0xFE,0x20}},
                                   {'5',{0x4E,0x8A,0x8A,0x8A,0x72}},
                                   {'6',{0x7C,0x92,0x92,0x92,0x64}},
                                   {'7',{0x06,0xC2,0x22,0x12,0x0E}},
                                   {'8',{0x6C,0x92,0x92,0x92,0x6C}},
                                   {'9',{0x0C,0x92,0x92,0x52,0x3C}},};
*/
// Bit


SSD1309::SSD1309()
{

    gpio_init(PIN_CS);

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state

    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    // init D/C pin
    gpio_init(PIN_DC);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_put(PIN_DC, 0);

    // init RES pin
    gpio_init(PIN_RES);
    gpio_set_dir(PIN_RES, GPIO_OUT);
    gpio_put(PIN_RES, 1);

}

//
//
//
void SSD1309::_sendCmd(uint8_t c)
{
    dc_command();
    cs_select();
    spi_write_blocking(SPI_PORT, &c, 1);
    cs_deselect();
}

//
//
//
void SSD1309::_sendData(uint8_t data[], uint16_t size)
{
    dc_data();
    cs_select();
    for (uint16_t i = 0; i<size; i++) {
        spi_write_blocking(SPI_PORT, &data[i], 1);
        sleep_us(5);
    }
    cs_deselect();
}

//
//
//
void SSD1309::_sendData(std::vector<uint8_t> data, uint16_t size)
{
    dc_data();
    cs_select();
    for (uint16_t i = 0; i<size; i++) {
        const uint8_t* constData = &data[i];
        spi_write_blocking(SPI_PORT, constData, 1);
        sleep_us(5);
    }
    cs_deselect();
}
//
//
//
void SSD1309::clrRAM()
{

    for (uint8_t p=0; p<8; ++p){
        _sendCmd(0xB0 | p);
        _sendCmd(0x00);
        _sendCmd(0x10);

        dc_data();
        cs_select();
        for (uint16_t i = 0; i<128; ++i) {
            spi_write_blocking(SPI_PORT, 0x00, 1);
            sleep_us(5);
        }
        cs_deselect();
    }
}

//
//
//
void SSD1309::drawFrame()
{
    uint8_t topBar[128];
    uint8_t botBar[128];
    uint8_t leftEdge[4] = {0x00,0xFF,0x00,0xFF};
    uint8_t rightEdge[4] = {0xFF,0x00,0xFF,0x00};

    for (uint16_t i = 0; i<128; ++i) {
        topBar[i] = 0x05;
        botBar[i] = 0x50;
    }


    for (uint8_t p=0; p<8; ++p){
        if (p==0){
            _sendCmd(0xB0);
            _sendCmd(0x00);
            _sendCmd(0x10);
            _sendData(topBar, 128);
        }
        else if (p==7){
            _sendCmd(0xB7);
            _sendCmd(0x00);
            _sendCmd(0x10);
            _sendData(botBar, 128);
        }
        /*
        else
        {
            _sendCmd(0xB0 | p);
            _sendCmd(0x00);
            _sendCmd(0x10);
            _sendData(leftEdge, 4);

            _sendCmd(0xB0 | p);
            _sendCmd(0x0B);
            _sendCmd(0x1F);
            _sendData(rightEdge, 4);

        }
        */
    }
}


//
//
//
void SSD1309::charTest()
{

    _sendCmd(0xB1);
    _sendCmd(0x00);
    _sendCmd(0x10);
    for (int i=0; i<27; i++)
    {
        if (i<18){
            _sendData(0x00, 1);
            _sendData(capFont_8x5[i], 5);
            _sendData(0x00, 1);
            sleep_ms(100);
        }
        else if (i == 18)
        {
            _sendCmd(0xB2);
            _sendCmd(0x00);
            _sendCmd(0x10);
            _sendData(0x00, 1);
            _sendData(capFont_8x5[i], 5);
            _sendData(0x00, 1);
            sleep_ms(100);
        }
        else
        {
            _sendData(0x00, 1);
            _sendData(capFont_8x5[i], 5);
            _sendData(0x00, 1);
            sleep_ms(100);
        }
    }

    _sendCmd(0xB3);
    _sendCmd(0x00);
    _sendCmd(0x10);
    for (int i=0; i<27; i++)
    {
        if (i<18){
            _sendData(0x00, 1);
            _sendData(lowFont_8x5[i], 5);
            _sendData(0x00, 1);
            sleep_ms(100);
        }
        else if (i == 18)
        {
            _sendCmd(0xB4);
            _sendCmd(0x00);
            _sendCmd(0x10);
            _sendData(0x00, 1);
            _sendData(lowFont_8x5[i], 5);
            _sendData(0x00, 1);
            sleep_ms(100);
        }
        else
        {
            _sendData(0x00, 1);
            _sendData(lowFont_8x5[i], 5);
            _sendData(0x00, 1);
            sleep_ms(100);
        }
    }

    _sendCmd(0xB5);
    _sendCmd(0x00);
    _sendCmd(0x10);
    for (int i=0; i<10; i++)
    {
        _sendData(0x00, 1);
        _sendData(numFont_8x5[i], 5);
        _sendData(0x00, 1);
        sleep_ms(100);
    }

}



//
//
//
void SSD1309::writeLine(char message[], int mSize, int line)
{
    switch(line){
    case 1:
        _sendCmd(0xB1);
        break;
    case 2:
        _sendCmd(0xB2);
        break;
    case 3:
        _sendCmd(0xB3);
        break;
    case 4:
        _sendCmd(0xB4);
        break;
    case 5:
        _sendCmd(0xB5);
        break;
    case 6:
        _sendCmd(0xB6);
        break;
    case 7:
        _sendCmd(0xB7);
        break;
    case 8:
        _sendCmd(0xB8);
        break;
    default:
        return;
    }
    _sendCmd(0x00);
    _sendCmd(0x10);
    sleep_us(5);

    int i = 0;
    //std::vector<uint8_t> charVec;
    char mChar;
    while ((i < 18) && (i < mSize)){
        mChar = message[i];
        //charVec = numMap.at(numMap.find(mChar));
        std::map<char, std::vector<uint8_t>>::const_iterator pos = charMap.find(mChar);
        if (pos == charMap.end()) {
            //handle the error
        } else {
            std::vector<uint8_t> charVec = pos->second;
            _sendData(0x00, 1);
            _sendData(charVec, 5);
            _sendData(0x00, 1);
            sleep_us(100);

        }
        i++;
    }
}


//
// Init Display
//
void SSD1309::init(void)
{

    //SPI.begin();

    _sendCmd(SSD1309_CMD_pwrOff);          //Display Off
    //_sendCmd(SSD1309_CMD_setContrastControl);       //Set contrast for color A
    //_sendCmd(0x91);                     //145
    //_sendCmd(CMD_MASTER_CURRENT_CONTROL);//master current control
    //_sendCmd(0x06);                     //6
    //_sendCmd(CMD_SET_PRECHARGE_SPEED_A);//Set Second Pre-change Speed For ColorA
    //_sendCmd(0x64);                     //100
    //_sendCmd(CMD_SET_PRECHARGE_SPEED_B);//Set Second Pre-change Speed For ColorB
    //_sendCmd(0x78);                     //120
    //_sendCmd(CMD_SET_PRECHARGE_SPEED_C);//Set Second Pre-change Speed For ColorC
    //_sendCmd(0x64);                     //100
    //_sendCmd(SSD1309_CMD_setSegmentMapReset);            //set remap & data format
    //_sendCmd(0x72);                     //0x72
    //_sendCmd(SSD1309_CMD_setSegmentMapFlipped);//Set display Start Line
    //_sendCmd(0x0);
    //_sendCmd(CMD_SET_DISPLAY_OFFSET);   //Set display offset
    //_sendCmd(0x0);
    //_sendCmd(CMD_NORMAL_DISPLAY);       //Set display mode
    //_sendCmd(SSD1309_CMD_setMultiplexRatio);  //Set multiplex ratio
    //_sendCmd(0x3F);
    //_sendCmd(CMD_SET_MASTER_CONFIGURE); //Set master configuration
    //_sendCmd(0x8E);
    //_sendCmd(CMD_POWER_SAVE_MODE);      //Set Power Save Mode
    //_sendCmd(0x00);                     //0x00
    //_sendCmd(CMD_PHASE_PERIOD_ADJUSTMENT);//phase 1 and 2 period adjustment
    //_sendCmd(0x31);                     //0x31
    //_sendCmd(CMD_DISPLAY_CLOCK_DIV);    //display clock divider/oscillator frequency
    //_sendCmd(0xF0);
    //_sendCmd(CMD_SET_PRECHARGE_VOLTAGE);//Set Pre-Change Level
    //_sendCmd(0x3A);
    //_sendCmd(CMD_SET_V_VOLTAGE);        //Set vcomH
    //_sendCmd(0x3E);
    //_sendCmd(CMD_DEACTIVE_SCROLLING);   //disable scrolling
    _sendCmd(SSD1309_CMD_setDisplayClockDivideRatio);    //display clock divider/oscillator frequency
    _sendCmd(0xF0);

    _sendCmd(SSD1309_CMD_pwrOn);
    sleep_ms(50);
    _sendCmd(SSD1309_CMD_allPixelsOn);
    //OLEDdriverObj.clearWindow(0,0,RGB_OLED_WIDTH-1,RGB_OLED_HEIGHT-1);
    sleep_ms(500);
    clrRAM();
    _sendCmd(SSD1309_CMD_followRAMcontent);
}

