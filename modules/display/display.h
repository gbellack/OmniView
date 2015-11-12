/* This file will contain the drivers for the 128x64 monochrome display.
 * WARNING: Only works on little endian systems!
 */

#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <stdint.h>

#define SSD1306_I2C_ADDRESS   0x3C	// 011 110+SA0+RW, SA0 is grounded.

/* PIN LAYOUT CONFIGURATIONS */

#define DISPLAY_RESET_PIN 	PIN_05 /* Pin 10 for display reset */

/* OLED CONFIGURATIONS */

#define SSD1306_LCDWIDTH                  	128	/* Display width */
#define SSD1306_LCDHEIGHT                 	64	/* Display height */

#define SSD1306_SETCONTRAST					0x8100
#define SSD1306_DISPLAYALLON_RESUME			0xA400
#define SSD1306_DISPLAYALLON 				0xA500
#define SSD1306_NORMALDISPLAY 				0xA600
#define SSD1306_INVERTDISPLAY 				0xA700
#define SSD1306_DISPLAYOFF 					0xAE00
#define SSD1306_DISPLAYON 					0xAF00

#define SSD1306_SETDISPLAYOFFSET 			0xD300
#define SSD1306_SETCOMPINS 					0xDA00

#define SSD1306_SETVCOMDETECT 				0xDB00

#define SSD1306_SETDISPLAYCLOCKDIV 			0xD500
#define SSD1306_SETPRECHARGE 				0xD900

#define SSD1306_SETMULTIPLEX 				0xA800

#define SSD1306_SETLOWCOLUMN				0x0000
#define SSD1306_SETHIGHCOLUMN 				0x1000

#define SSD1306_SETSTARTLINE 				0x4000

#define SSD1306_MEMORYMODE 					0x2000
#define SSD1306_COLUMNADDR 					0x2100
#define SSD1306_PAGEADDR   					0x2200

#define SSD1306_COMSCANINC 					0xC000
#define SSD1306_COMSCANDEC 					0xC800

#define SSD1306_SEGREMAP 					0xA100

#define SSD1306_CHARGEPUMP 					0x8D00

#define SSD1306_EXTERNALVCC 				0x0100
#define SSD1306_SWITCHCAPVCC 				0x0200

/* SCROLLING DEFINES */
#define SSD1306_ACTIVATE_SCROLL 						0x2F00
#define SSD1306_DEACTIVATE_SCROLL 						0x2E00
#define SSD1306_SET_VERTICAL_SCROLL_AREA				0xA300
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 				0x2600
#define SSD1306_LEFT_HORIZONTAL_SCROLL 					0x2700
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 	0x2900
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 	0x2A00

#define BLACK 0
#define WHITE 1
#define INVERSE 2

/* NAME PARAMETERS */
#define MAX_NAMES_ALLOCATED 5
#define MAX_NAME_LENGTH 15
#define NAME_TEXT_SIZE 1

#define TEXT_COLOR WHITE

/* Swaps the two values */
#define swap(a, b) { int16_t t = a; a = b; b = t; }
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

/* EFFECTS: Displays the name on the screen. */
extern void DisplayName(const char *firstName, const char *lastName);

/* Printing functions */
extern void print(const char *str);
extern void println(const char *str);
extern void PrintHelper(uint8_t c);

extern void FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
extern void DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
extern void DrawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color);
extern void DrawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color);

// These exist only with Adafruit_GFX (no subclass overrides)
extern void DrawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
extern void SetCursor(int16_t x, int16_t y);
extern void SetTextColor(uint16_t c);
extern void SetTextSize(uint8_t s);
extern void SetTextWrap(uint8_t w);

/* REQUIRES: The reset pin.
 * EFFECTS: Initializes the display pins.
 */
extern void InitializeDisplay();

/* EFFECTS: Send and receive the payload */
extern void Send(uint16_t payload);

/*
 * Display
 *
 */

/* EFFECTS: Clears the display. */
extern void ClearDisplay();

/* EFFECTS: Displays whatever that is stored in buffer */
extern void Display();

/* EFFECTS: Dims the display */
extern void Dim(int dim);

/* EFFECTS: Turns the display on. */
extern void DisplayOn();

/* EFFECTS:d Turns off the display. */
extern void DisplayOff();

/*
 * Scrolling
 *
 */

/* EFFECTS: Sets the scroll direction */
extern void StartScrollRight(uint8_t start, uint8_t stop);

extern void StartScrollLeft(uint8_t start, uint8_t stop);

extern void StopScroll();

/*
 * DRAWING
 *
 */

/* REQUIRES: the x, y, and color.
 * EFFECTS: Sets a pixel.
 */
extern void DrawPixel(int16_t x, int16_t y, uint16_t color);

#endif
