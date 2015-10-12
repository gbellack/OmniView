//*****************************************************************************
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

// Standard includes.
#include <stdio.h>
#include <stdlib.h>

#include "osi.h"

// Driverlib includes
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_types.h"
#include "hw_ints.h"
#include "interrupt.h"
#include "rom.h"
#include "rom_map.h"
#include "uart.h"
#include "prcm.h"
#include "utils.h"

// Common interface includes
#include "uart_if.h"

#include "pinmux.h"

//*****************************************************************************
//                      MACRO DEFINITIONS
//*****************************************************************************
#define APPLICATION_VERSION     "1.1.1"
#define UART_PRINT              Report
#define SPAWN_TASK_PRIORITY     9
#define OSI_STACK_SIZE          2048
#define APP_NAME                "FreeRTOS Demo"
#define MAX_MSG_LENGTH			16

// CAMERA
#define CAMERA_TASK_STACK_SIZE 2048
#define CAMERA_TASK_NAME "Camera Task"
#define CAMERA_TASK_PRIORITY 1

// DISPLAY
#define DISPLAY_TASK_STACK_SIZE 2048
#define DISPLAY_TASK_NAME "Display Task"
#define DISPLAY_TASK_PRIORITY 1

//*****************************************************************************
//                      PROTOTYPES
//*****************************************************************************
static void initializeBoard();
static void cameraTask( void *pvParameters );
static void displayTask( void *pvParameters );

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
// The queue used to send strings to the task1.
OsiMsgQ_t MsgQ;

#ifndef USE_TIRTOS
/* in case of TI-RTOS don't include startup_*.c in app project */
#if defined(gcc) || defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//*****************************************************************************
// FreeRTOS User Hook Functions enabled in FreeRTOSConfig.h
//*****************************************************************************
#ifdef USE_FREERTOS

	//*****************************************************************************
	//
	//! \brief Application defined hook (or callback) function - assert
	//!
	//! \param[in]  pcFile - Pointer to the File Name
	//! \param[in]  ulLine - Line Number
	//!
	//! \return none
	//!
	//*****************************************************************************
	void vAssertCalled( const char *pcFile, unsigned long ulLine )
	{
		//Handle Assert here
		while(1);
	}

	void vApplicationIdleHook( void)
	{
		//Handle Idle Hook for Profiling, Power Management etc
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

#endif //USE_FREERTOS

// EFFECTS: Initializes the board.
static void initializeBoard(void)
{
	/* In case of TI-RTOS vector table is initialize by OS itself */
	#ifndef USE_TIRTOS
		//
		// Set vector table base
		//
		#if defined(ccs) || defined(gcc)
			MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
		#endif

		#if defined(ewarm)
			MAP_IntVTableBaseSet((unsigned long)&__vector_table);
		#endif

	#endif

	//
	// Enable Processor
	//
	MAP_IntMasterEnable();
	MAP_IntEnable(FAULT_SYSTICK);

	PRCMCC3200MCUInit();
}

// EFECTS: Prints out the displayed content on the screen.
void displayTask( void *pvParameters )
{
   unsigned long ul_2;
   const char *pcInterruptMessage[4] = {"Welcome","to","CC32xx","development !\n"};

   ul_2 =0;

   for( ;; )
   {
		/* Queue a message for the print task to display on the UART CONSOLE. */
		osi_MsgQWrite(&MsgQ, (void*) pcInterruptMessage[ul_2 % 4], OSI_NO_WAIT);
		ul_2++;
		osi_Sleep(200);
   }
}

// EFFECTS: Takes a picture.
void cameraTask( void *pvParameters )
{
	char pcMessage[MAX_MSG_LENGTH];
	for( ;; )
	{
		/* Wait for a message to arrive. */
		osi_MsgQRead(&MsgQ, pcMessage, OSI_WAIT_FOREVER);

		UART_PRINT("message = ");
		UART_PRINT(pcMessage);
		UART_PRINT("\n\r");
		osi_Sleep(200);
	}
}

int main( void )
{
    initializeBoard();

    PinMuxConfig();

    //
    // Initializing the terminal
    //
    InitTerm();

    //
    // Clearing the terminal
    //
    ClearTerm();

    //
    // Creating a queue for 10 elements.
    //
    OsiReturnVal_e osi_retVal;
    osi_retVal = osi_MsgQCreate(&MsgQ, "MSGQ", MAX_MSG_LENGTH, 10);
    if(osi_retVal != OSI_OK)
    {
    	// Queue was not created and must not be used.
    	while(1);
    }

    // Create the Queue Receive task
    osi_TaskCreate(displayTask, DISPLAY_TASK_NAME, DISPLAY_TASK_STACK_SIZE,
    				NULL, CAMERA_TASK_PRIORITY, NULL);

    // Create the Queue Send task
    osi_TaskCreate(cameraTask, CAMERA_TASK_NAME, CAMERA_TASK_STACK_SIZE,
    				NULL, CAMERA_TASK_PRIORITY, NULL );

    // Start the task scheduler
    osi_start();

    return 0;
}
