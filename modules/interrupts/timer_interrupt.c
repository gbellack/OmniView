/* This file contains the timer interrupt interface
 * Created on: November 22, 2015
 * Zheng Hao Tan
 */

#include "timer_interrupt.h"

//Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "timer.h"

//Common interface includes
#include "gpio_if.h"
#include "common.h"
#ifndef NOTERM
#include "uart_if.h"
#endif
#include "../../pinmux.h"
#include "timer_if.h"

#include <inttypes.h>

// Variable used in Timer Interrupt Handler
uint8_t g_usTimerInts;

/* EFFECTS: Initializes Timer A and enables it */
void TimerConfigNStart()
{
  // Configure Timer for blinking the LED for IP acquisition
  Timer_IF_Init(PRCM_TIMERA0, TIMERA0_BASE, TIMER_CFG_PERIODIC, TIMER_A, 0);
  Timer_IF_IntSetup(TIMERA0_BASE, TIMER_A, TimerPeriodicIntHandler);
  Timer_IF_Start(TIMERA0_BASE, TIMER_A, TIME_IN_MSECS);
}

/* EFFECTS: Deinitializes Timer A and unregisters it */
void TimerDeinitStop()
{
  // Disable the LED blinking Timer as Device is connected to AP
  Timer_IF_Stop(TIMERA0_BASE, TIMER_A);
  Timer_IF_DeInit(TIMERA0_BASE, TIMER_A);
}

/* EFFECTS: Timer Interrupt handler, which toggles the RED led */
void TimerPeriodicIntHandler(void)
{
    unsigned long ulInts;

    // Clear all pending interrupts from the timer we are currently using.
    ulInts = MAP_TimerIntStatus(TIMERA0_BASE, true);
    MAP_TimerIntClear(TIMERA0_BASE, ulInts);

    // Increment our interrupt counter.
    if(g_usTimerInts)
    {
//        GPIO_IF_LedOff(MCU_RED_LED_GPIO);
        g_usTimerInts = 0;
    }
    else
    {
//        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        g_usTimerInts = 1;
    }
}
