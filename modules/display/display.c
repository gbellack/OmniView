#include "display.h"
#include "display_driver.h"

#include <stdio.h>
#include <stdlib.h>
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

// Common interface includes
#ifndef NOTERM
	#include "uart_if.h"
#endif

#include "i2c_if.h"


void DisplayTask(void *pvParameters) {

    InitializeDisplay();
	ClearDisplay();
	Display();

//	// Show image buffer on the display hardware.
//	// Since the buffer is intialized with an Adafruit splashscreen
//	// internally, this will display the splashscreen.
//	display.display();
//	delay(2000);
//
//	// Clear the buffer.
//	display.clearDisplay();
//
//	// draw a single pixel
//	display.drawPixel(10, 10, WHITE);
//	// Show the display buffer on the hardware.
//	// NOTE: You _must_ call display after making any drawing commands
//	// to make them visible on the display hardware!
//	display.display();
//	delay(2000);
//	display.clearDisplay();
	for (;;) {

	}
}
