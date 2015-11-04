#include "display.h"
#include "display_driver.h"
#include "display_network.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Common interface includes
#ifndef NOTERM
	#include "uart_if.h"
#endif

#include "i2c_if.h"

/* NAME PARAMETERS */
#define MAX_NAMES_ALLOCATED 5
#define MAX_NAME_LENGTH 15
#define NAME_TEXT_SIZE 2

#define TEXT_COLOR WHITE

/* Array of first and last names */
char firstName[MAX_NAMES_ALLOCATED][MAX_NAME_LENGTH];
char lastName[MAX_NAMES_ALLOCATED][MAX_NAME_LENGTH];

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
