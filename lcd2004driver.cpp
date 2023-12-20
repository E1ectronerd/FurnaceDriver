#include "lcd2004driver.h"

/* Quick helper function for single byte transfers */
void i2c_write_byte(uint8_t val) {
#ifdef i2c_default
    i2c_write_blocking(i2c_default, addr, &val, 1, false);
#endif
}

void lcd_toggle_enable(uint8_t val) {
    // Toggle enable pin on LCD display
    // We cannot do this too quickly or things don't work
#define DELAY_US 600
    sleep_us(DELAY_US);
    i2c_write_byte(val | LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
    i2c_write_byte(val & ~LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
}

// The display is sent a byte as two separate nibble transfers
void lcd_send_byte(uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;
    //uint8_t high = mode | (val & 0xF0);
    //uint8_t low = mode | ((val << 4) & 0xF0);
    i2c_write_byte(high);
    lcd_toggle_enable(high);
    i2c_write_byte(low);
    lcd_toggle_enable(low);
}

void lcd_clear(void) {
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

// go to location on LCD
void lcd_set_cursor(int line, int position) {
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(val, LCD_COMMAND);
}

static void inline lcd_char(char val) {
    lcd_send_byte(val, LCD_CHARACTER);
}

void lcd_string(const char *s) {
    while (*s) {
        lcd_char(*s++);
    }
}

void lcd_init() {
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear();
}


//
//
//
LCD2004Driver::LCD2004Driver()
{

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear();
    lcd_send_byte(LCD_CURSORON, LCD_COMMAND);
    lcd_send_byte(LCD_BLINKON, LCD_COMMAND);

}

//
//
//
void LCD2004Driver::writeTest()
{
    lcd_send_byte(LCD_WRITE_LINE_ONE,LCD_COMMAND);
    lcd_send_byte('1',LCD_CHARACTER);
    lcd_send_byte(LCD_WRITE_LINE_TWO,LCD_COMMAND);
    lcd_send_byte('2',LCD_CHARACTER);
    lcd_send_byte(LCD_WRITE_LINE_THREE,LCD_COMMAND);
    lcd_send_byte('3',LCD_CHARACTER);
    lcd_send_byte(LCD_WRITE_LINE_FOUR,LCD_COMMAND);
    lcd_send_byte('4',LCD_CHARACTER);
}

//
//
//
void LCD2004Driver::writeLine(char message[], int mSize, int line)
{
    switch(line){
    case 1:
        lcd_send_byte(LCD_WRITE_LINE_ONE,LCD_COMMAND);
        sleep_us(5);
        break;
    case 2:
        lcd_send_byte(LCD_WRITE_LINE_TWO,LCD_COMMAND);
        sleep_us(5);
        break;
    case 3:
        lcd_send_byte(LCD_WRITE_LINE_THREE,LCD_COMMAND);
        sleep_us(5);
        break;
    case 4:
        lcd_send_byte(LCD_WRITE_LINE_FOUR,LCD_COMMAND);
        sleep_us(5);
        break;
    default:
        return;
    }

    for (int i = 0; i < 20; i++) {
        if (i < mSize)
        {
            lcd_send_byte(message[i], LCD_CHARACTER);
        }
        else
        {
            lcd_send_byte(' ', LCD_CHARACTER);
        }
    }
    //lcd_send_byte(LCD_RETURNHOME, LCD_COMMAND);

}








