#include "display_driver.h"

/* Common includes */
#include <stdint.h>
#include <string.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "utils.h"
#include "uart.h"
#include "gpio.h"
#include "gpio_if.h"

#include "../../pinmux.h"
#include "hw_gpio.h"
#include "pin.h"

// Common interface includes
#ifndef NOTERM
	#include "uart_if.h"
#endif
#include "i2c_if.h"

#define UART_PRINT	Report

/* Buffer that stores the image. */
static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] = {

	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

/* EFFECTS: Initializes the I2C pins and set up the display configurations. */
void InitializeDisplay() {

	UART_PRINT("Starting Initialize Display\n\r");

    /* CC3200 - I2C Init to fast mode */
    I2C_IF_Open(I2C_MASTER_MODE_FST);

    UART_PRINT("Opened I2C\n\r");

	// Power on sequence.
	#ifdef RESET_I2C_DISPLAY_PIN

		// Configure reset pin and set its direction.
		MAP_PinTypeGPIO(DISPLAY_RESET_PIN, PIN_MODE_0, false);
		MAP_GPIODirModeSet(GPIOA1_BASE, 0x4, GPIO_DIR_MODE_OUT);

		/* Set RESET PIN to high for GPIO output. */
		GPIOPinWrite(GPIOA1_BASE, DISPLAY_RESET_PIN, 1);

		// TODO - Figure out delays (might need freeRTOS implementation here)...
		// VDD (3.3V) goes high at start, lets just chill for a ms
        MAP_UtilsDelay(10000);

		/* Bring reset low */
		GPIOPinWrite(GPIOA1_BASE, DISPLAY_RESET_PIN, 0);

		// wait 10ms
        MAP_UtilsDelay(100000);

		// bring out of reset
		GPIOPinWrite(GPIOA1_BASE, DISPLAY_RESET_PIN, 1);

	#endif

	/* Init sequence for 128x64 OLED module */
	Send(SSD1306_DISPLAYOFF);                    // 0xAE

	/* Set display clock divide/oscillator frequency. */
	Send(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
	Send(0x8000);                                // the suggested ratio 0x80

	/* Set multiplex ratio */
	Send(SSD1306_SETMULTIPLEX);                  // 0xA8
	Send(0x3F00);

	/* Set display offset */
	Send(SSD1306_SETDISPLAYOFFSET);              // 0xD3
	Send(0x0000);                                // no offset

	/* Setting starting line address */
	Send(SSD1306_SETSTARTLINE);            // line #0

	/* Charge pump */
	Send(SSD1306_CHARGEPUMP);                    // 0x8D
	Send(0x1400);

    Send(SSD1306_MEMORYMODE);                    // 0x20
    Send(0x0000);                                  // 0x0 act like ks0108

    Send(SSD1306_SEGREMAP);
    Send(SSD1306_COMSCANDEC);

	/* Set comp pins hardware configuration */
    Send(SSD1306_SETCOMPINS);                    // 0xDA
    Send(0x1200);

	/* Set contrast */
    Send(SSD1306_SETCONTRAST);                   // 0x81
    Send(0xCF00);

    /* Set pre charge period */
    Send(SSD1306_SETPRECHARGE);                  // 0xd9
	Send(0xF100);

	/* Set vcomh */
    Send(SSD1306_SETVCOMDETECT);                 // 0xDB
    Send(0x4000);

    Send(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    Send(SSD1306_NORMALDISPLAY);                 // 0xA6

    DisplayOn();
}

/* EFFECTS: Send and receive the payload */
void Send(uint16_t payload) {

	I2C_IF_Write(SSD1306_I2C_ADDRESS, &payload, 2, 1);
}

/* EFFECTS: Clears the display. */
void ClearDisplay() {

		memset(buffer, 0, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8));

		Send(SSD1306_COLUMNADDR);
		Send(0x0000);   // Column start address (0 = reset)
		Send(0x7F00);

		Send(SSD1306_PAGEADDR);
		Send(0x0000); // Page start address (0 = reset)
		Send(0x0700); // Page end address

		uint16_t i = 0;

		for (i = 0; i < 64; ++i) {
			uint8_t temp[17] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			I2C_IF_Write(SSD1306_I2C_ADDRESS, temp, 17, 1);
		}
}

/* EFFECTS: Displays whatever that is stored in buffer */
void Display() {

	Send(SSD1306_COLUMNADDR);
	Send(0x0000);   // Column start address (0 = reset)
	Send(0x7F00); // Column end address (127 = reset)

	Send(SSD1306_PAGEADDR);
	Send(0x0000); // Page start address (0 = reset)
	Send(0x0700); // Page end address

	uint16_t i = 0;

	for (i = 0; i < 64; ++i) {
		// send a bunch of data in one xmission

		uint8_t temp[17] = {0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
		I2C_IF_Write(SSD1306_I2C_ADDRESS, temp, 17, 1);
	}
    MAP_UtilsDelay(10000);
}

void InvertDisplay() {
//	if (i) {
//		Send(SSD1306_INVERTDISPLAY);
//	}
//	else {
//		Send(SSD1306_NORMALDISPLAY);
//	}
}

/* EFFECTS: Turns the display on. */
void DisplayOn() {
	Send(SSD1306_DISPLAYON);
}

/* EFFECTS: Turns off the display. */
void DisplayOff() {
	Send(SSD1306_DISPLAYOFF);
}

void StartScrollRight(uint8_t start, uint8_t stop) {

	Send(SSD1306_RIGHT_HORIZONTAL_SCROLL);
	Send(0x00);
	Send(start);
	Send(0x00);
	Send(stop);
	Send(0x00);
	Send(0xFF);
	Send(SSD1306_ACTIVATE_SCROLL);
}

void StartScrollLeft(uint8_t start, uint8_t stop) {

	Send(SSD1306_LEFT_HORIZONTAL_SCROLL);
	Send(0x00);
	Send(start);
	Send(0x00);
	Send(stop);
	Send(0x00);
	Send(0xFF);
	Send(SSD1306_ACTIVATE_SCROLL);
}

void StopScroll() {
	Send(SSD1306_DEACTIVATE_SCROLL);
}

void Dim(int dim) {

//	uint8_t contrast;
//
//	if (dim) {
//		contrast = 0; // Dimmed display
//	}
//	else {
//		if (_vccstate == SSD1306_EXTERNALVCC) {
//			contrast = 0x9F;
//		}
//		else {
//			contrast = 0xCF;
//		}
//	}
//
//	// the range of contrast to too small to be really useful
//	// it is useful to dim the display
//	Send(SSD1306_SETCONTRAST);
//	Send(contrast);
}

/* REQUIRES: the x, y, and color.
 * EFFECTS: Sets a pixel based on its coordinate.
 */
void DrawPixel(int16_t x, int16_t y, uint16_t color) {

	/* Input validation - Check if x and y are within bounds */
	if ((x < 0) || (x >= SSD1306_LCDWIDTH) || (y < 0) || (y >= SSD1306_LCDHEIGHT))
		return;

	switch (color)
	{
		case WHITE:
			buffer[x+ (y/8)*SSD1306_LCDWIDTH] |=  (1 << (y & 7));
			break;
		case BLACK:
			buffer[x+ (y/8)*SSD1306_LCDWIDTH] &= ~(1 << (y & 7));
			break;
		case INVERSE:
			buffer[x+ (y/8)*SSD1306_LCDWIDTH] ^=  (1 << (y&7));
			break;
	}
}
