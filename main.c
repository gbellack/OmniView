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
#define MCU_ENTER_DEEPSLEEP_TASK_STACK_SIZE 2048
#define MCU_ENTER_DEEPSLEEP_TASK_NAME "Enter Deepsleep Task"
#define MCU_ENTER_DEEPSLEEP_TASK_PRIORITY 1

#define MCU_EXIT_DEEPSLEEP_TASK_STACK_SIZE 2048
#define MCU_EXIT_DEEPSLEEP_TASK_NAME "Exit Deepsleep Task"
#define MCU_EXIT_DEEPSLEEP_TASK_PRIORITY 1

#define MCU_ENTER_HIBERNATE_TASK_STACK_SIZE 2048
#define MCU_ENTER_HIBERNATE_TASK_NAME "Enter Hibernate Task"
#define MCU_ENTER_HIBERNATE_TASK_PRIORITY 1

#define MCU_EXIT_HIBERNATE_TASK_STACK_SIZE 2048
#define MCU_EXIT_HIBERNATE_TASK_NAME "Exit Hibernate Task"
#define MCU_EXIT_HIBERNATE_TASK_PRIORITY 1

// MICROPHONE
#define MICROPHONE_TASK_STACK_SIZE 2048
#define MICROPHONE_TASK_NAME "Microphone Task"
#define MICROPHONE_TASK_PRIORITY 1

/* STATUS TASK */
#define STATUS_TASK_STACK_SIZE 2048
#define STATUS_TASK_NAME "Status Check Task"
#define STATUS_TASK_PRIORITY 10

void TimerPeriodicIntHandler();

static void InitializeBoard();

// The queue used to send strings to the task1.
OsiMsgQ_t MsgQ;

/* We are using this for FREERTOS */
extern void (* const g_pfnVectors[])(void);

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

// EFFECTS: Initializes the board.
static void InitializeBoard()
{
	MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);

	// Enable Processor
	MAP_IntMasterEnable();
	MAP_IntEnable(FAULT_SYSTICK);

	PRCMCC3200MCUInit();
}

/* EFFECTS: Checks the tasks and ends them if they are inconsistent */
void CheckStatusTask(void *pvParameters) {

}

int main( void )
{
    InitializeBoard();

    // Configure pins.
    PinMuxConfig();

	#ifndef NOTERM
		InitTerm();
	    ClearTerm();
	#endif

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

//	// Create the Queue Camera task
//	osi_TaskCreate(CameraTask, CAMERA_TASK_NAME, CAMERA_TASK_STACK_SIZE,
//					NULL, CAMERA_TASK_PRIORITY, NULL);

//    // Create the Queue Wireless task
//    lRetVal = osi_TaskCreate(WlanAPModeTask, WIRELESS_AP_TASK_NAME, WIRELESS_AP_TASK_STACK_SIZE,
//			NULL, WIRELESS_AP_TASK_PRIORITY, NULL);

//    if(lRetVal < 0)
//    {
//        ERR_PRINT(lRetVal);
//        LOOP_FOREVER();
//    }

    // Start the task scheduler
    osi_start();

    return 0;
}
