/*

A library to control SSD1309 OLED drivers, derived from the SparkFun HyperDisplay library

Created: August 2018
Modified: August 2018

Authors: Owen Lyke

*/

#ifndef SSD1309_H
#define SSD1309_H


////////////////////////////////////////////////////////////
//							Includes    				  //
////////////////////////////////////////////////////////////
#include <map>
#include <vector>

//#include "OLEDdriver.h"

#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
////////////////////////////////////////////////////////////
//							Defines     				  //
////////////////////////////////////////////////////////////
#define SSD1309_MAX_WIDTH 	128
#define SSD1309_MAX_HEIGHT 	64
#define SSD1309_START_ROW 	0
#define SSD1309_START_COL 	0
#define SSD1309_START_PG 	0
#define SSD1309_STOP_ROW 	63
#define SSD1309_STOP_COL 	127
#define SSD1309_STOP_PG		7
#define LOW 0
#define HIGH 1

#define SSD1309_RAM_BYTES (SSD1309_MAX_WIDTH*(SSD1309_MAX_HEIGHT/8))

// #define SSD1309_DEFAULT_WORKING_NUM_BYTES 2*64

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MOSI 3   // SCA Main Out Subnode In
#define PIN_MISO 4   // Not used for display
#define PIN_CS   5   // Chip Select - board pin 7
#define PIN_SCK  2   // D0 Clock SCLK - board pin 3
#define PIN_DC   6   // Data/Command 1=data 0=command  - board pin 6
#define PIN_RES  7   // Reset 0=reset 1=operate - board pin 5

typedef void * color_t;

template <typename T>
T nspoti(T i1){					// Nearest smaller power of two for integers
	T retval = i1;				// Be explicit about passing by value
	T bytesize = sizeof( T );	// Number of bytes (assuming 8 bits) that the type occupies
	retval--; 					// In case we start on a power of two
	if(retval == 0){ return 0; }
	if(bytesize > 0){
		retval |= (retval >> 1);
		retval |= (retval >> 2);
		retval |= (retval >> 4);
	}
	if(bytesize > 1){
		retval |= (retval >> 8);
	}
	if(bytesize > 3){
		retval |= (retval >> 16);
	}
	if(bytesize > 4){
		return 0;
	}
	retval++;				// This will carry a one into the next highest bit - we now have the nearest greater power of two (thanks http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2Float)
	retval = retval >> 1; 	// This divides by two to get the nearest lower power of two
	// retval += (retval == 0); // If you needed you could add this line to make sure that 0 is not returned
	return retval;
}


//
//
//
typedef struct character_info{
    void *            	data;		        // deprecated - fonts now use the first color in the window's color sequence
    uint16_t*  	xLoc;		       			// x location data relative to the upper left-corner of the character area
    uint16_t*	yLoc;		       			// y location data relative to the upper left-corner of the character area
    uint16_t    xDim;                  		// The maximum value of xLoc
    uint16_t    yDim;                  		// The maximum value of yLoc - also the number of pixels to move down for characters that cause new lines
    uint32_t    		numPixels;           		// The number of color_t types that pdata points to
    bool            	show;				    	// Whether or not to actually show the character
    bool            	causesNewline;				// This indicates if the given chracter is meant to cause a newline
}char_info_t;                           	        // Character information structure for placing pixels in a window


//
//
//
typedef struct window_info{
    uint16_t  xMin;                  	// FYI window min/max use the hardware frame of reference
    uint16_t  xMax;						//
    uint16_t  yMin;						//
    uint16_t  yMax;						//
    double     cursorX;               		// Where the cursor is currently in window-coordinates
    double     cursorY;               		// Where the cursor is currently in window-coordinates
    double     xReset;            			// Where the cursor goes on a reset location (window coordinates)
    double     yReset;            			// Where the cursor goes on a reset location (window coordinates)
    char_info_t     lastCharacter;      	  	// Information about the last character written.
    void *         currentSequenceData;		// The data that is used as the default color sequence
    uint32_t     currentColorCycleLength;	// The default color sequence number of pixels
    uint32_t     currentColorOffset;			// The current offset
    bool            bufferMode;                 // Indicates either buffer or direct mode (direct is default)
    void *         data;                       // A pointer to pixel data that is specific to the window. Can be left as NULL
    uint32_t		numPixels;					// The number of pixel types that data points to
    bool            dynamic;                    // Indicator if the current buffer memory was dynamically allocated - so that it can be disposed of automatically
}wind_info_t;                       		    // Window infomation structure for placing objects on the display


// Number Lookup Map
const std::map<char, std::vector<uint8_t>> charMap{{' ',{0x00,0x00,0x00,0x00,0x00}},
                                                  {'a',{0x60,0x94,0x94,0x94,0xF8}},
                                                  {'A',{0xF8,0x24,0x22,0x24,0xF8}},
                                                  {'s',{0x10,0xA8,0xA8,0x40,0x00}},
                                                  {'S',{0x4C,0x92,0x92,0x92,0x64}},
                                                  {'d',{0x60,0x90,0x90,0x90,0xFE}},
                                                  {'D',{0xFE,0x82,0x82,0x82,0x7C}},
                                                  {'f',{0x10,0xFC,0x12,0x12,0x00}},
                                                  {'F',{0xFE,0x12,0x12,0x02,0x02}},
                                                  {'j',{0x40,0x80,0x88,0x7A,0x08}},
                                                  {'J',{0x40,0x82,0x82,0x7E,0x02}},
                                                  {'k',{0xFE,0x10,0x28,0xC4,0x00}},
                                                  {'K',{0xFE,0x10,0x38,0x44,0x82}},
                                                  {'l',{0x7C,0x80,0x80,0x00,0x00}},
                                                  {'L',{0xFE,0x80,0x80,0x80,0x00}},
                                                  {';',{0x00,0x80,0x64,0x00,0x00}},
                                                  {':',{0x00,0x00,0x24,0x00,0x00}},
                                                  {'-',{0x00,0x10,0x10,0x10,0x00}},
                                                  {'.',{0x00,0x00,0x80,0x00,0x00}},
                                                  {'g',{0x18,0xA4,0xA4,0xA4,0x7C}},
                                                  {'G',{0x7C,0x82,0x92,0x92,0xE4}},
                                                  {'h',{0xFE,0x30,0x10,0x10,0xE0}},
                                                  {'H',{0xFE,0x10,0x10,0x10,0xFE}},
                                                  {'0',{0x7C,0x82,0xBA,0x82,0x7C}},
                                                  {'1',{0x84,0x84,0xFE,0x80,0x80}},
                                                  {'2',{0x84,0xC2,0xA2,0x92,0x8C}},
                                                  {'3',{0x82,0x92,0x92,0x92,0x6C}},
                                                  {'4',{0x30,0x28,0x24,0xFE,0x20}},
                                                  {'5',{0x4E,0x8A,0x8A,0x8A,0x72}},
                                                  {'6',{0x7C,0x92,0x92,0x92,0x64}},
                                                  {'7',{0x06,0xC2,0x22,0x12,0x0E}},
                                                  {'8',{0x6C,0x92,0x92,0x92,0x6C}},
                                                  {'9',{0x0C,0x92,0x92,0x52,0x3C}},
                                                  {'q',{0x30,0x48,0x48,0x48,0xF8}},
                                                  {'Q',{0x7C,0x82,0xA2,0x42,0xBC}},
                                                  {'w',{0x70,0x80,0x60,0x80,0x70}},
                                                  {'W',{0x7E,0x80,0x70,0x80,0x7E}},
                                                  {'e',{0x78,0x94,0x94,0x94,0x18}},
                                                  {'E',{0xFE,0x92,0x92,0x92,0x82}},
                                                  {'r',{0x00,0xF0,0x20,0x10,0x10}},
                                                  {'R',{0xFE,0x12,0x12,0x32,0xCC}},
                                                  {'t',{0x08,0x7C,0x88,0x88,0x00}},
                                                  {'T',{0x02,0x02,0xFE,0x02,0x02}},
                                                  {'y',{0x98,0xA0,0x60,0x18,0x00}},
                                                  {'Y',{0x06,0x08,0xF0,0x08,0x06}},
                                                  {'u',{0x70,0x80,0x80,0x40,0xF0}},
                                                  {'U',{0x7E,0x80,0x80,0x80,0x7E}},
                                                  {'i',{0x00,0x90,0xF4,0x80,0x00}},
                                                  {'I',{0x00,0x82,0xFE,0x82,0x00}},
                                                  {'o',{0x70,0x88,0x88,0x88,0x70}},
                                                  {'O',{0x7C,0x82,0x82,0x82,0x7C}},
                                                  {'p',{0xF8,0x48,0x48,0x48,0x30}},
                                                  {'P',{0xFE,0x12,0x12,0x12,0x0C}},
                                                  {'v',{0x30,0x40,0x80,0x40,0x30}},
                                                  {'V',{0x1E,0x60,0x80,0x60,0x1E}},
                                                  {'b',{0xFE,0x90,0x90,0x90,0x60}},
                                                  {'B',{0xFE,0x92,0x92,0x92,0x6C}},
                                                  {'c',{0x70,0x88,0x88,0x88,0x10}},
                                                  {'C',{0x7C,0x82,0x82,0x82,0x44}},
                                                  {'n',{0xE0,0x20,0x10,0x10,0xE0}},
                                                  {'N',{0xFE,0x04,0x08,0x10,0xFE}},
                                                  {'m',{0xE0,0x10,0x60,0x10,0xE0}},
                                                  {'M',{0xFE,0x04,0x08,0x04,0xFE}},
                                                  {'x',{0x88,0x50,0x20,0x50,0x88}},
                                                  {'X',{0xC6,0x28,0x10,0x28,0xC6}},
                                                  {'z',{0xC8,0xA8,0x98,0x88,0x00}},
                                                  {'Z',{0xC2,0xA2,0x92,0x8A,0x86}},
                                                  };

////////////////////////////////////////////////////////////
//							Typedefs    				  //
////////////////////////////////////////////////////////////
typedef enum{
	SSD1309_Nominal = 0,
	SSD1309_Error,
	SSD1309_NoRefresh,
	SSD1309_NotImplemented,

	SSD1309_NumStatus
}SSD1309_Status_t;

typedef enum{
	SSD1309_Intfc_4WSPI = 0,
	SSD1309_Intfc_3WSPI,
	SSD1309_Intfc_I2C,
	SSD1309_Intfc_8080,
	SSD1309_Intfc_6800,
}SSD1309_Intfc_t;		// This lists the possible interface modes of the SSD1309. The mode is selected by strapping pins BS[2:0] and the binary representations of these enum values corresponds to those strapping pins

typedef union{
	struct{
		uint8_t b0 : 1;	// Bit 0
		uint8_t b1 : 1; // Bit 1
		uint8_t b2 : 1; // Bit 2
		uint8_t b3 : 1; // Bit 3
		uint8_t b4 : 1; // Bit 4
		uint8_t b5 : 1; // Bit 5
		uint8_t b6 : 1; // Bit 6
		uint8_t b7 : 1; // Bit 7
	};
	uint8_t bite; // All bits arranged in order
}SSD1309_Bite_t;	// The basic (minimal) type that holds pixel information 

typedef enum{
	// Fundamental Commands
	SSD1309_CMD_setContrastControl = 0x81,
	SSD1309_CMD_followRAMcontent = 0xA4,
	SSD1309_CMD_allPixelsOn = 0xA5,
	SSD1309_CMD_inversionOff = 0xA6,
	SSD1309_CMD_inversionOn = 0xA7,
	SSD1309_CMD_pwrOff = 0xAE, 
	SSD1309_CMD_pwrOn = 0xAF,
	SSD1309_CMD_nop = 0xE3,
	SSD1309_CMD_setCommandLock = 0xFD,

	// Scrolling Command Table
	SSD1309_CMD_contHScrollSetupRight = 0x26,
	SSD1309_CMD_contHScrollSetupLeft = 0x27,
	SSD1309_CMD_contVHScrollSetupRight = 0x29,
	SSD1309_CMD_contVHScrollSetupLeft = 0x2A,
	SSD1309_CMD_deactivateScroll = 0x2E,
	SSD1309_CMD_activateScroll = 0x2F,
	SSD1309_CMD_setVScrollArea = 0xA3,
	SSD1309_CMD_contentScrollSetupRight = 0x2C,
	SSD1309_CMD_contentScrollSetupLeft = 0x2D,

	// Addressing Setting Command Table
	SSD1309_CMD_setLowCSAinPAM = 0x00, 		// Note: this command should be OR'd with the desired Page Address Mode Lower Nibble of Column Start Address when it is sent
	SSD1309_CMD_setHighCSAinPAM = 0x10,		// This command also OR'd with the high nibble...
	SSD1309_CMD_setMemoryAddressingMode = 0x20, // Only for horizontal or vertical addressing mode...
	SSD1309_CMD_setColumnAddress = 0x21,
	SSD1309_CMD_setPageAddress = 0x22,
	SSD1309_CMD_setPSAinPAM = 0xB0, 		// Note: OR this with 3 Least Significant bits that represent the page start address
	SSD1309_CMD_setDisplayStartLine = 0x40, // Note: OR this with 6 Least Significant Bits that represent the display start line
	SSD1309_CMD_setSegmentMapReset = 0xA0,
	SSD1309_CMD_setSegmentMapFlipped = 0xA1,
	SSD1309_CMD_setMultiplexRatio = 0xA8,	
	SSD1309_CMD_setCOMoutputNormal = 0xC0,
	SSD1309_CMD_setCOMoutputFlipped = 0xC8,
	SSD1309_CMD_setDisplayOffset = 0xD3,
	SSD1309_CMD_setCOMpinsHWconfig = 0xDA,
	SSD1309_CMD_setGPIO	= 0xDC,

	// Timing and Driving Scheme Setting Command Table
	SSD1309_CMD_setDisplayClockDivideRatio = 0xD5,
	SSD1309_CMD_setPreChargePeriod = 0xD9,
	SSD1309_CMD_setVCOMHdeselectLevel = 0xDB,
}SSD1309_Cmd_t;

////////////////////////////////////////////////////////////
//					 Class Definition   				  //
////////////////////////////////////////////////////////////
class SSD1309 {
private:
protected:



public: // temporary
    SSD1309();


public:
    // display init Process
    void init(void);

	// Here is the main API implementation that allows this class to hook into the hyperdisplay library
    void hwpixel(uint8_t x0, uint8_t y0, void * data = NULL, uint32_t colorCycleLength = 1, uint32_t startColorOffset = 0); 																																	// Single pixel write. Required by hyperdisplay. Uses screen-relative coordinates
	// void hwxline(hd_hw_extent_t x0, hd_hw_extent_t y0, hd_hw_extent_t len, color_t data = NULL, hd_colors_t colorCycleLength = 1, hd_colors_t startColorOffset = 0, bool goLeft = false); 																						// More efficient xline imp. Uses screen-relative coordinates
	// void hwyline(hd_hw_extent_t x0, hd_hw_extent_t y0, hd_hw_extent_t len, color_t data = NULL, hd_colors_t colorCycleLength = 1, hd_colors_t startColorOffset = 0, bool goUp = false); 																							// More efficient yline imp. Uses screen-relative coordinates
	// void hwrectangle(hd_hw_extent_t x0, hd_hw_extent_t y0, hd_hw_extent_t x1, hd_hw_extent_t y1, bool filled = false, color_t data = NULL, hd_colors_t colorCycleLength = 1, hd_colors_t startColorOffset = 0, bool reverseGradient = false, bool gradientVertical = false); 	// More efficient rectangle imp in window-relative coordinates
	// void hwfillFromArray(hd_hw_extent_t x0, hd_hw_extent_t y0, hd_hw_extent_t x1, hd_hw_extent_t y1, color_t data = NULL, hd_pixels_t numPixels = 0,  bool Vh = false );	 																																									// More efficient fill from array implementation. Uses screen-relative coordinates

    // Write Command Byte
    void _sendCmd(uint8_t cmd);
    // Write Data Bytes
    void _sendData(uint8_t data[], uint16_t size);
    void _sendData(std::vector<uint8_t> data, uint16_t size);
    //void writeBytes(uint8_t* data, uint8_t numBytes);

    void clrRAM();
    static inline void cs_select() {
        asm volatile("nop \n nop \n nop");
        gpio_put(PIN_CS, 0);  // Active low
        asm volatile("nop \n nop \n nop");
    }

    static inline void cs_deselect() {
        asm volatile("nop \n nop \n nop");
        gpio_put(PIN_CS, 1);
        asm volatile("nop \n nop \n nop");
    }

    static inline void dc_data() {
        asm volatile("nop \n nop \n nop");
        gpio_put(PIN_DC, 1);
        asm volatile("nop \n nop \n nop");
    }

    static inline void dc_command() {
        asm volatile("nop \n nop \n nop");
        gpio_put(PIN_DC, 0);
        asm volatile("nop \n nop \n nop");
    }

    // Draw frame
    void drawFrame();

    //
    void charTest();

    //
    void writeLine(char message[], int mSize, int line);
};

#endif //SSD1309_H
