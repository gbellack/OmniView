#ifndef BUTTON_INTERRUPT_H
#define BUTTON_INTERRUPT_H

/* This file contains the button interrupt interface used for switching between
 * face and speech recognition modes.
 *
 * Zheng Hao Tan
 */

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

/* This is the amount of time to count to account for button debouncing. */
#define BUTTON_DEBOUNCE_THRESHOLD 4

/* This determines the button interrupt priority */
#define BUTTON_INTERRUPT_PRIORITY 10

/* Macros for interrupt button pins */
#define INTERRUPT_BUTTON_PIN 				PIN_01
#define INTERRUPT_BUTTON_GPIO_PIN 			GPIO_PIN_2
#define INTERRUPT_BUTTON_BASE_ADDR 			GPIOA1_BASE
#define INTERRUPT_BUTTON_GPIO_HW_INT 		INT_GPIOA1

/* EFFECTS: Initializes all the interrupts used in this application */
extern void InitializeInterrupts();

/* EFFECTS: Disable and unregister all interrupts */
extern void DeinitializeInterrupts();

/* EFFECTS: Toggles the mode based on button press */
static void ButtonPressIntHandler(void);

#endif
