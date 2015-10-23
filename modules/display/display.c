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

	for (;;) {
//		ClearDisplay();
//		Display();
	}
}
