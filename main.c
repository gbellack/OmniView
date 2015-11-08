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
#include "systick.h"
#include "pin.h"
#include "simplelink.h"

#ifndef NOTERM
	#include "uart_if.h"
#endif
#include "udma_if.h"

#include "common.h"
#include "pinmux.h"

#include "modules/camera/camera.h"
#include "modules/display/display.h"
#include "modules/display/display_driver.h"
#include "modules/mcu/mcu.h"
#include "modules/microphone/microphone.h"
#include "modules/wireless/wireless.h"
#include "httpserverapp.h"

//*****************************************************************************
//                      MACRO DEFINITIONS
//*****************************************************************************
#define UART_PRINT              Report
#define SPAWN_TASK_PRIORITY     9

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

/* MICROPHONE */
#define MICROPHONE_TASK_STACK_SIZE 2048
#define MICROPHONE_TASK_NAME "Microphone Task"
#define MICROPHONE_TASK_PRIORITY 1

/* STATUS TASK */
#define STATUS_TASK_STACK_SIZE 2048
#define STATUS_TASK_NAME "Status Check Task"
#define STATUS_TASK_PRIORITY 8

/* Prototypes */
void TimerPeriodicIntHandler();
static void InitializeBoard();

// The queue used to send strings to the task1.
OsiMsgQ_t MsgQ;


//*****************************************************************************
//                      GLOBAL VARIABLES for VECTOR TABLE
//*****************************************************************************
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

//*****************************************************************************
//                          LOCAL DEFINES
//*****************************************************************************
#define APP_NAME		        "WebSocket"
#define SPAWN_TASK_PRIORITY     9
#define HTTP_SERVER_APP_TASK_PRIORITY  1
#define OSI_STACK_SIZE          2048

//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************

#ifndef NOTERM
static void
DisplayBanner(char * AppName)
{

    Report("\n\n\n\r");
    Report("\t\t *************************************************\n\r");
    Report("\t\t	  CC3200 %s Application       \n\r", AppName);
    Report("\t\t *************************************************\n\r");
    Report("\n\n\n\r");
}
#endif


//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs) || defined(gcc)
    IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);
    PRCMCC3200MCUInit();
}



/* We are using this for FREERTOS */
extern void (* const g_pfnVectors[])(void);

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
    BoardInit();
    UDMAInit();
    // Configure pins.
    PinMuxConfig();

	#ifndef NOTERM
		InitTerm();
	    ClearTerm();
	    DisplayBanner(APP_NAME);
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

    osi_TaskCreate(HttpServerAppTask,
                    "WebSocketApp",
                        OSI_STACK_SIZE,
                        NULL,
                        HTTP_SERVER_APP_TASK_PRIORITY,
                        NULL );

//	// Create the Queue Camera task
//	osi_TaskCreate(CameraTask, CAMERA_TASK_NAME, CAMERA_TASK_STACK_SIZE,
//					NULL, CAMERA_TASK_PRIORITY, NULL);

    // Create the Queue Wireless task
//    lRetVal = osi_TaskCreate(WlanAPModeTask, WIRELESS_AP_TASK_NAME,
//								WIRELESS_AP_TASK_STACK_SIZE, NULL,
//								WIRELESS_AP_TASK_PRIORITY, NULL);

//    if(lRetVal < 0)
//    {
//        ERR_PRINT(lRetVal);
//        LOOP_FOREVER();
//    }

    // Start the task scheduler
    osi_start();

    return 0;
}


//****************************************************************************
//						WEBSOCKET CAMERA MAIN FUNCTION
//****************************************************************************
/*void main() {

	//
	// Board Initialization
	//
	BoardInit();

	//
	// Enable and configure DMA
	//
	UDMAInit();
	//
	// Pinmux for UART
	//
	PinMuxConfig();

#ifndef NOTERM
	//
	// Configuring UART
	//
	InitTerm();

    //
    // Display Application Banner
    //
    DisplayBanner(APP_NAME);
#endif
    //
    // Start the SimpleLink Host
    //
    VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    //
    // Start the HttpServer Task
    //
    //

    osi_TaskCreate(HttpServerAppTask,
                    "WebSocketApp",
                        OSI_STACK_SIZE,
                        NULL,
                        HTTP_SERVER_APP_TASK_PRIORITY,
                        NULL );

    UART_PRINT("HttpServerApp Initialized \n\r");

}

*/
