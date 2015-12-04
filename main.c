#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "osi.h"

/* DRIVERLIB INCLUDES */
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

#include "mode.h"

//*****************************************************************************
//                      MACRO DEFINITIONS
//*****************************************************************************
#define SPAWN_TASK_PRIORITY     9
#define MAX_MSG_LENGTH 16

#define FACE_RECOGNITION_TASK_NAME "FaceRecognitionTask"
#define FACE_RECOGNITION_TASK_STACK_SIZE 8192
//#define FACE_RECOGNITION_TASK_STACK_SIZE 2048
#define FACE_RECOGNITION_TASK_PRIORITY 2

/* Prototypes */
static void InitializeBoard();

// The queue used to send strings to the task1.
OsiMsgQ_t MsgQ;

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

int main()
{
    InitializeBoard();

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

	osi_TaskCreate(FaceRecognitionMode, FACE_RECOGNITION_TASK_NAME, FACE_RECOGNITION_TASK_STACK_SIZE,
					NULL, FACE_RECOGNITION_TASK_PRIORITY, NULL);

    // Start the task scheduler
    osi_start();

    return 0;
}
