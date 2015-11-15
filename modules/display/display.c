#include "display.h"

#include "font.c"
#include "buffer.c"

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

#define UART_PRINT Report

// GFX variables.
int16_t cursor_x, cursor_y;
uint16_t textcolor, textbgcolor;
uint8_t textsize, rotation, wrap;

void DisplayName(const char *firstName, const char *lastName) {
	DisplayPrintLine(firstName);
	DisplayPrint("  ");
	DisplayPrintLine(lastName);
}

void DisplayPrint(const char *str) {
	int i = 0;
	while (str[i] != '\0') {
		PrintHelper(str[i]);
		++i;
	}
}

void DisplayPrintLine(const char *str) {
	DisplayPrint(str);
	IncrementLine();
}

void PrintHelper(uint8_t c) {

	// TODO
	/* If new line escape or if x is too far away */
	if (c == '\n') {
		IncrementLine();
	}
	else if (c == '\r') {
	// skip em
	}
	else {
		DrawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);

		cursor_x += textsize * 6;

		if (wrap && (cursor_x > ((SSD1306_LCDWIDTH / 2) - textsize * 6))) {
			IncrementLine();
		}
	}
}

void IncrementLine() {
	cursor_y += textsize*8;
	cursor_x = 0;
}

void FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

	int16_t i = 0;
	for (i = x; i < x + w; ++i) {
		DrawFastVLine(i, y, h, color);
	}
}

void DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {

	uint8_t bSwap = 0;

	switch (rotation) {
		case 0:
		 	break;
		case 1:
			// 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
			bSwap = true;
			swap(x, y);
			x = SSD1306_LCDWIDTH - x - 1;
			x -= (h-1);
			break;
		case 2:
			// 180 degree rotation, invert x and y - then shift y around for height.
			x = SSD1306_LCDWIDTH - x - 1;
			y = SSD1306_LCDHEIGHT - y - 1;
			y -= (h - 1);
			break;
		case 3:
			// 270 degree rotation, swap x & y for rotation, then invert y
			bSwap = true;
			swap(x, y);
			y = SSD1306_LCDHEIGHT - y - 1;
			break;
	}

	if (bSwap) {
		DrawFastHLineInternal(x, y, h, color);
	}
	else {
		DrawFastVLineInternal(x, y, h, color);
	}
}

void DrawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color) {

  // do nothing if we're off the left or right side of the screen
  if(x < 0 || x >= SSD1306_LCDWIDTH) {
  	return;
  }

  // make sure we don't try to draw below 0
  if(__y < 0) {
    // __y is negative, this will subtract enough from __h to account for __y being 0
    __h += __y;
    __y = 0;

  }

  // make sure we don't go past the height of the display
  if( (__y + __h) > SSD1306_LCDHEIGHT) {
    __h = (SSD1306_LCDHEIGHT - __y);
  }

  // if our height is now negative, punt
  if(__h <= 0) {
    return;
  }

  // this display doesn't need ints for coordinates, use local byte registers for faster juggling
  register uint8_t y = __y;
  register uint8_t h = __h;

  // set up the pointer for fast movement through the buffer
  register uint8_t *pBuf = buffer;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * SSD1306_LCDWIDTH);
  // and offset x columns in
  pBuf += x;

  // do the first partial byte, if necessary - this requires some masking
  register uint8_t mod = (y&7);
  if(mod) {
    // mask off the high n bits we want to set
    mod = 8-mod;

    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    // register uint8_t mask = ~(0xFF >> (mod));
    static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
    register uint8_t mask = premask[mod];

    // adjust the mask if we're not going to reach the end of this byte
    if( h < mod) {
      mask &= (0XFF >> (mod-h));
    }

  switch (color)
    {
    case WHITE:   *pBuf |=  mask;  break;
    case BLACK:   *pBuf &= ~mask;  break;
    case INVERSE: *pBuf ^=  mask;  break;
    }

    // fast exit if we're done here!
    if(h<mod) { return; }

    h -= mod;

    pBuf += SSD1306_LCDWIDTH;
  }

  // write solid bytes while we can - effectively doing 8 rows at a time
  if(h >= 8) {
    if (color == INVERSE)  {          // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
      do  {
      *pBuf=~(*pBuf);

        // adjust the buffer forward 8 rows worth of data
        pBuf += SSD1306_LCDWIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
      }
    else {
      // store a local value to work with
      register uint8_t val = (color == WHITE) ? 255 : 0;

      do  {
        // write our value in
      *pBuf = val;

        // adjust the buffer forward 8 rows worth of data
        pBuf += SSD1306_LCDWIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
      }
    }

	// now do the final partial byte, if necessary
	if (h) {

		mod = h & 7;
		// this time we want to mask the low bits of the byte, vs the high bits we did above
		// register uint8_t mask = (1 << mod) - 1;
		// note - lookup table results in a nearly 10% performance improvement in fill* functions
		static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
		register uint8_t mask = postmask[mod];
		switch (color)
		{
			case WHITE:   *pBuf |=  mask;  break;
			case BLACK:   *pBuf &= ~mask;  break;
			case INVERSE: *pBuf ^=  mask;  break;
	    }
	}
}

void DrawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) {

    // Do bounds/limit checks
    if( y < 0 || y >= SSD1306_LCDHEIGHT) {
        return;
    }

    // make sure we don't try to draw below 0
    if(x < 0) {
        w += x;
        x = 0;
    }

    // make sure we don't go off the edge of the display
    if( (x + w) > SSD1306_LCDWIDTH) {
    w = (SSD1306_LCDWIDTH - x);
    }

    // if our SSD1306_LCDWIDTH is now negative, punt
    if(w <= 0) { return; }

    // set up the pointer for  movement through the buffer
    register uint8_t *pBuf = buffer;
    // adjust the buffer pointer for the current row
    pBuf += ((y / 8) * SSD1306_LCDWIDTH);
    // and offset x columns in
    pBuf += x;

    register uint8_t mask = 1 << (y&7);

  switch (color)
  {
	case WHITE:
	  while(w--) { *pBuf++ |= mask; }; break;
    case BLACK: mask = ~mask;   while(w--) { *pBuf++ &= mask; }; break;
  case INVERSE:         while(w--) { *pBuf++ ^= mask; }; break;
  }
}

// Draw a character
void DrawChar(int16_t x, int16_t y, unsigned char c,
			    uint16_t color, uint16_t bg, uint8_t size) {

  if((x >= SSD1306_LCDWIDTH)            || // Clip right
     (y >= SSD1306_LCDHEIGHT)           || // Clip bottom
     ((x + 6 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;

  int8_t i = 0;
  for (i = 0; i < 6; i++ ) {
    uint8_t line;
    if (i == 5)
      line = 0x0;
    else
      line = pgm_read_byte(font + (c * 5) + i);

    int8_t j = 0;
    for (j = 0; j < 8; ++j) {
      if (line & 0x1) {
        if (size == 1) // default size
          DrawPixel(x + i, y + j, color);
        else {  // big size
          FillRect(x + (i * size), y + (j * size), size, size, color);
        }
      }
      else if (bg != color) {
        if (size == 1) // default size
          DrawPixel(x + i, y + j, bg);
        else {  // big size
          FillRect(x + i*size, y + j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

void SetCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
}

void SetTextSize(uint8_t s) {
    textsize = (s > 0) ? s : 1;
}

void SetTextColor(uint16_t c) {
	// For 'transparent' background, we'll set the bg
	// to the same as fg instead of using a flag
	textcolor = textbgcolor = c;
}

void SetTextWrap(uint8_t w) {
  wrap = w;
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

	/* Set the cursor back to the original position */
	SetCursor(0, SSD1306_LCDHEIGHT - textsize*8);
}

/* EFFECTS: Initializes the I2C pins and set up the display configurations. */
void InitializeDisplay() {

    /* CC3200 - I2C Init to fast mode */
    I2C_IF_Open(I2C_MASTER_MODE_FST);

    rotation  = 0;
    cursor_y = cursor_x = 0;
    textsize = 1;
    textcolor = textbgcolor = 0xFFFF;
    wrap = 1;

	// Power on sequence.
	#ifdef RESET_I2C_DISPLAY_PIN

		/* Set RESET PIN to high for GPIO output. */
		GPIOPinWrite(GPIOA1_BASE, DISPLAY_RESET_PIN, 1);

		// TODO - Figure out delays (might need freeRTOS implementation here)...
		// VDD (3.3V) goes high at start, lets just chill for a ms
    	UtilsDelay(10000);

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
    /* Set the text size and color */
	SetTextSize(NAME_TEXT_SIZE);
	SetTextColor(WHITE);
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

	/* Set the cursor back to the original position */
	SetCursor(0, SSD1306_LCDHEIGHT/2);// - textsize*8);
}

/* EFFECTS: Displays whatever that is stored in buffer */
void Display() {

	Send(SSD1306_COLUMNADDR);
	Send(0x0000);   // Column start address (0 = reset)
	Send(0x7F00); // Column end address (127 = reset)

	Send(SSD1306_PAGEADDR);
	Send(0x0000); // Page start address (0 = reset)
	Send(0x0700); // Page end address

	uint8_t tempBuffer[17] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	uint16_t i = 0;
	for (i = 0; i < 1024; ++i) {

		// Copy all over to the buffer.
		memcpy(tempBuffer + 1, buffer + i, 16);

		I2C_IF_Write(SSD1306_I2C_ADDRESS, tempBuffer, 17, 1);
		i += 15;
	}
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
//		contrast = 0xCF;
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
			buffer[x+ (y/8)*SSD1306_LCDWIDTH] ^=  (1 << (y & 7));
			break;
	}
}
