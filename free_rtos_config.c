#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Simplelink includes
#include "simplelink.h"

// driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "interrupt.h"
#include "timer.h"
#include "rom.h"
#include "rom_map.h"
#include "utils.h"
#include "prcm.h"

// free-rtos/TI-rtos include
#include "osi.h"

// common interface includess
#include "common.h"
#ifndef NOTERM
#include "uart_if.h"
#endif
#include "timer_if.h"

//*****************************************************************************
// FreeRTOS User Hook Functions enabled in FreeRTOSConfig.h
//*****************************************************************************
//! \brief Application defined hook (or callback) function - assert
//!
//! \param[in]  pcFile - Pointer to the File Name
//! \param[in]  ulLine - Line Number
void vAssertCalled( const char *pcFile, unsigned long ulLine )
{
	//Handle Assert here
	while(1);
}

void vApplicationIdleHook()
{
	//Handle Idle Hook for Profiling, Power Management etc

    int iRetVal;

    //
    // Enter SLEEP...WaitForInterrupt ARM intrinsic
    //
    DBG_PRINT("DEEPSLEEP: Entering DeepSleep\n\r");

    //MAP_UtilsDelay(80000);
    for(iRetVal = 0; iRetVal < 80000; iRetVal++);

    //
    // Disable the SYSTICK interrupt
    //
    MAP_IntDisable(FAULT_SYSTICK);
    MAP_PRCMDeepSleepEnter();

    //
    // Enable the SYSTICK interrupt
    //
    MAP_IntEnable(FAULT_SYSTICK);
    DBG_PRINT("DEEPSLEEP: Exiting DeepSleep\n\r");

}

void vApplicationMallocFailedHook()
{
	//Handle Memory Allocation Errors
	while(1);
}

void vApplicationStackOverflowHook( OsiTaskHandle *pxTask,
								   signed char *pcTaskName)
{
	//Handle FreeRTOS Stack Overflow
	while(1);
}
