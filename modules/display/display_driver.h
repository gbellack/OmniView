/* This file will contain the drivers for the 128x64 monochrome display. */

#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <stdint.h>

#define SSD1306_I2C_ADDRESS   0x3C	// 011110+SA0+RW, SA0 is grounded.

/* PIN LAYOUT CONFIGURATIONS */

#define DISPLAY_RESET_PIN 	PIN_17 /* Pin 10 for camera reset */
#define DISPLAY_SCL_PIN 	PIN_01 /* Pin 1 for I2C scl */
#define DISPLAY_SDA_PIN 	PIN_02 /* Pin 2 for I2c sda */

/* OLED CONFIGURATIONS */

#define SSD1306_LCDWIDTH                  	128	/* Display width */
#define SSD1306_LCDHEIGHT                 	64	/* Display height */

#define SSD1306_SETCONTRAST					0x81
#define SSD1306_DISPLAYALLON_RESUME			0xA4
#define SSD1306_DISPLAYALLON 				0xA5
#define SSD1306_NORMALDISPLAY 				0xA6
#define SSD1306_INVERTDISPLAY 				0xA7
#define SSD1306_DISPLAYOFF 					0xAE
#define SSD1306_DISPLAYON 					0xAF

#define SSD1306_SETDISPLAYOFFSET 			0xD3
#define SSD1306_SETCOMPINS 					0xDA

#define SSD1306_SETVCOMDETECT 				0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 			0xD5
#define SSD1306_SETPRECHARGE 				0xD9

#define SSD1306_SETMULTIPLEX 				0xA8

#define SSD1306_SETLOWCOLUMN				0x00
#define SSD1306_SETHIGHCOLUMN 				0x10

#define SSD1306_SETSTARTLINE 				0x40

#define SSD1306_MEMORYMODE 					0x20
#define SSD1306_COLUMNADDR 					0x21
#define SSD1306_PAGEADDR   					0x22

#define SSD1306_COMSCANINC 					0xC0
#define SSD1306_COMSCANDEC 					0xC8

#define SSD1306_SEGREMAP 					0xA0

#define SSD1306_CHARGEPUMP 					0x8D

#define SSD1306_EXTERNALVCC 				0x1
#define SSD1306_SWITCHCAPVCC 				0x2

#define SSD1306_ACTIVATE_SCROLL 						0x2F
#define SSD1306_DEACTIVATE_SCROLL 						0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA				0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 				0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 					0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 	0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 	0x2A

#define BLACK 0
#define WHITE 1
#define INVERSE 2

/* Swaps the two values */
#define ssd1306_swap(a, b) { int16_t t = a; a = b; b = t; }

/* REQUIRES: The reset pin.
 * EFFECTS: Initializes the display pins.
 */
extern void InitializeDisplay();

/* EFFECTS: Send and receive the payload */
extern void Send(uint8_t payload);

/*
 * Display
 *
 */

/* EFFECTS: Clears the display. */
extern void ClearDisplay();

/* EFFECTS: Displays whatever that is stored in buffer */
extern void Display();

/* EFFECTS: Inverts the display. */
extern void InvertDisplay();

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
