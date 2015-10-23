#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "osi.h"

// Driverlib includes
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_types.h"
#include "hw_ints.h"
#include "interrupt.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "uart.h"
#include "utils.h"

#ifndef NOTERM
	#include "uart_if.h"
#endif

#include "common.h"
#include "pinmux.h"

#include "modules/camera/camera.h"
#include "modules/display/display.h"
#include "modules/display/display_driver.h"
#include "modules/mcu/mcu.h"
#include "modules/microphone/microphone.h"
#include "modules/wireless/wireless.h"

//*****************************************************************************
//                      MACRO DEFINITIONS
//*****************************************************************************
#define UART_PRINT              Report
#define SPAWN_TASK_PRIORITY     9
#define OSI_STACK_SIZE          2048
#define APP_NAME                "OmniView"
#define MAX_MSG_LENGTH			16

// CAMERA
#define CAMERA_TASK_STACK_SIZE 2048
#define CAMERA_TASK_NAME "Camera Task"
#define CAMERA_TASK_PRIORITY 1

// DISPLAY
#define DISPLAY_TASK_STACK_SIZE 2048
#define DISPLAY_TASK_NAME "Display Task"
#define DISPLAY_TASK_PRIORITY 1

// WIRELESS
#define WIRELESS_AP_TASK_STACK_SIZE 2048
#define WIRELESS_AP_TASK_NAME "Wireless Task"
#define WIRELESS_AP_TASK_PRIORITY 2

// MCU
#define MCU_TASK_STACK_SIZE 2048
#define MCU_TASK_NAME "MCU Task"
#define MCU_TASK_PRIORITY 1

// MICROPHONE
#define MICROPHONE_TASK_STACK_SIZE 2048
#define MICROPHONE_TASK_NAME "Microphone Task"
#define MICROPHONE_TASK_PRIORITY 1

static void InitializeBoard();

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
// FreeRTOS User Hook Functions enabled in FreeRTOSConfig.h
//*****************************************************************************
#ifdef USE_FREERTOS

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
static void InitializeBoard()
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

	// Enable Processor
	MAP_IntMasterEnable();
	MAP_IntEnable(FAULT_SYSTICK);

	PRCMCC3200MCUInit();
}

int main( void )
{
    InitializeBoard();

    // Configure pins.
    PinMuxConfig();

	#ifndef NOTERM
		InitTerm();
	#endif

    ClearTerm();

    // Creating a queue for 10 elements.
    OsiReturnVal_e osi_retVal;
    osi_retVal = osi_MsgQCreate(&MsgQ, "MSGQ", MAX_MSG_LENGTH, 10);
    if(osi_retVal != OSI_OK)
    {
    	// Queue was not created and must not be used.
    	while(1);
    }

	long lRetVal = -1;

	// Start the SimpleLink Host
	lRetVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
	if(lRetVal < 0)
	{
		ERR_PRINT(lRetVal);
		LOOP_FOREVER();
	}

	// Create the Queue Display task
	osi_TaskCreate(DisplayTask, DISPLAY_TASK_NAME, DISPLAY_TASK_STACK_SIZE,
					NULL, CAMERA_TASK_PRIORITY, NULL);

	// Create the Queue Camera task
	osi_TaskCreate(CameraTask, CAMERA_TASK_NAME, CAMERA_TASK_STACK_SIZE,
					NULL, CAMERA_TASK_PRIORITY, NULL);

//    // Create the Queue Wireless task
//    lRetVal = osi_TaskCreate(WlanAPModeTask, WIRELESS_AP_TASK_NAME, WIRELESS_AP_TASK_STACK_SIZE,
//			NULL, WIRELESS_AP_TASK_PRIORITY, NULL);

    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    // Start the task scheduler
    osi_start();

    return 0;
}
