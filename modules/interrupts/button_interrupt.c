#include <stdlib.h>

//Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "timer.h"

//Free_rtos/ti-rtos includes
#include "osi.h"

//Common interface includes
#include "gpio_if.h"
#include "common.h"

#include "button_interrupt.h"
#include <inttypes.h>
#include "gpio.h"
#include "pin.h"

//Display includes
#include "utils.h"
#include "rom_map.h"
#include "rom.h"
#include "../display/display.h"

uint8_t toggleFaceRecognitionMode = 0; /* Toggle mode between face and speech recognition */

volatile uint8_t count = 0;

/* EFFECTS: Toggles the mode based on button press */
static void ButtonPressIntHandler(void)
{
	unsigned long ulPinState =  GPIOIntStatus(INTERRUPT_BUTTON_BASE_ADDR, 1);

	/* False alarms */
	if(!(ulPinState & INTERRUPT_BUTTON_GPIO_PIN)) {
		count = 0;
		return;
	}
	else if (count < BUTTON_DEBOUNCE_THRESHOLD) {
		++count;
		return;
	}
	else {
		count = 0;
	}

	/* Clear and Disable GPIO Interrupt */
    MAP_GPIOIntDisable(INTERRUPT_BUTTON_BASE_ADDR, INTERRUPT_BUTTON_GPIO_PIN);
    MAP_GPIOIntClear(INTERRUPT_BUTTON_BASE_ADDR, INTERRUPT_BUTTON_GPIO_PIN);
    MAP_IntDisable(INTERRUPT_BUTTON_GPIO_HW_INT);


    ClearDisplay();
    DisplayPrintLine("Button Pressed");
    Display();
    MAP_UtilsDelay(1000000);

/*
	// OmniView Modes
	if (toggleFaceRecognitionMode)
	{
//		task1();
		toggleFaceRecognitionMode = 0;
	}
	else // Speech recognition mode
	{
//		task2();
		toggleFaceRecognitionMode = 1;
	}
*/

	/* Enable GPIO Interrupt */
	MAP_GPIOIntClear(INTERRUPT_BUTTON_BASE_ADDR, INTERRUPT_BUTTON_GPIO_PIN);
	MAP_IntPendClear(INTERRUPT_BUTTON_GPIO_HW_INT);
	MAP_IntEnable(INTERRUPT_BUTTON_GPIO_HW_INT);
	MAP_GPIOIntEnable(INTERRUPT_BUTTON_BASE_ADDR, INTERRUPT_BUTTON_GPIO_PIN);
}

/* EFFECTS: Initializes all the interrupts used in this application */
void InitializeInterrupts() {

	MAP_GPIOIntTypeSet(INTERRUPT_BUTTON_BASE_ADDR, INTERRUPT_BUTTON_GPIO_PIN,
						GPIO_FALLING_EDGE);

	short retVal = osi_InterruptRegister(INTERRUPT_BUTTON_GPIO_HW_INT, ButtonPressIntHandler,
											BUTTON_INTERRUPT_PRIORITY);
	if (retVal != OSI_OK) {

	}

	MAP_GPIOIntClear(INTERRUPT_BUTTON_BASE_ADDR, INTERRUPT_BUTTON_GPIO_PIN);
	MAP_GPIOIntEnable(INTERRUPT_BUTTON_BASE_ADDR, INTERRUPT_BUTTON_GPIO_PIN);
}

/* EFFECTS: Disable and unregister all interrupts */
void DeinitializeInterrupts() {

	osi_InterruptDeRegister(INTERRUPT_BUTTON_GPIO_HW_INT);
}
