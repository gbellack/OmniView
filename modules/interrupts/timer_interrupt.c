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
#include "../microphone/microphone.h"
//Common interface includes
#include "gpio.h"
#include "gpio_if.h"
#include "common.h"
#ifndef NOTERM
#include "uart_if.h"
#endif
#include "../../pinmux.h"
#include "timer_if.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>

// Variable used in Timer Interrupt Handler
uint8_t g_usTimerInts;

bool RecordingDone;
uint32_t CurrentSamples = 0;
static uint32_t TotalSamples = 0;
static uint16_t *RecordingBuf;

/* EFFECTS: Initializes Timer A0 and enables it
 * 			Then collects the amount of ADC samples as specified
 * 			in sampleRequest. Stores these samples in buf */
//need a sample rate of 10 Khz
void TimerConfigNStart(uint32_t sampleRequest, char *buf)
{
  // Configure Timer for blinking the LED for IP acquisition
  TotalSamples = sampleRequest;
  CurrentSamples = 0;
  RecordingDone = false;
  RecordingBuf = (uint16_t*)buf;
  Timer_IF_Init(PRCM_TIMERA0, TIMERA0_BASE, TIMER_CFG_PERIODIC, TIMER_A, 0);
  Timer_IF_IntSetup(TIMERA0_BASE, TIMER_A, TimerPeriodicIntHandler);

  //now IF_START last argument is the interrupt frequency in Hz
  Timer_IF_Start(TIMERA0_BASE, TIMER_A, SAMPLE_RATE);
}

/* EFFECTS: Deinitializes Timer A and unregisters it */
void TimerDeinitStop()
{
  // Disable the LED blinking Timer as Device is connected to AP
  Timer_IF_Stop(TIMERA0_BASE, TIMER_A);
  Timer_IF_DeInit(TIMERA0_BASE, TIMER_A);
}

/* EFFECTS: Timer Interrupt handler, which collects microphone data */
void TimerPeriodicIntHandler(void)
{
	//gpio 9
	//GPIOPinWrite(GPIOA1_BASE, 0x02, 0xFF);
    unsigned long ulInts;

    // Clear all pending interrupts from the timer we are currently using.
    ulInts = MAP_TimerIntStatus(TIMERA0_BASE, true);
    MAP_TimerIntClear(TIMERA0_BASE, ulInts);

    if(CurrentSamples >= TotalSamples){
    	RecordingDone = true;
    	return;
    }

    //grab microphone current sample
    RecordingBuf[CurrentSamples] = GetMicSample();
    CurrentSamples++;

    // Increment our interrupt counter.
   // GPIOPinWrite(GPIOA1_BASE, 0x02, 0x00);
}
