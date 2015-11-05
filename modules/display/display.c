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


/* Array of first and last names */
//char firstName[MAX_NAMES_ALLOCATED][MAX_NAME_LENGTH];
//char lastName[MAX_NAMES_ALLOCATED][MAX_NAME_LENGTH];

void DisplayName(const char *firstName, const char *lastName) {
	println(firstName);
	println(lastName);
}

void DisplayTask(void *pvParameters) {

    InitializeDisplay();
	ClearDisplay();

	// text display tests
	SetTextSize(NAME_TEXT_SIZE);
	SetTextColor(WHITE);
	SetCursor(0,0);

	DisplayName("GARRISON", "BELLACK");
	DisplayName("ZHENG HAO", "TAN");
	DisplayName("TZU-FEI", "YU");
	DisplayName("LOREN", "WANG");
	DisplayName("JOHN", "CONNOLLY");

	Display();

	for (;;) {

	}
}
