/* This file contains the modes for the interrupt handler function.
 * When the button gets pressed, this part will change OmniView into
 * face recognition mode or name recording mode. */
#include "mode.h"

#include "rom_map.h"

#include "rom.h"
/* INIT INCLUDES */
#include "pinmux.h"
#include "udma_if.h"

/* DISPLAY INCLUDES */
#include "modules/display/display.h"

#include "modules/mcu/mcu.h"

/* MICROPHONE INCLUDES */
#include "modules/microphone/microphone.h"

/* CAMERA INCLUDES */
#include "modules/camera/camera_app.h"
#include "modules/camera/i2cconfig.h"

/* INTERRUPT INCLUDES */
#include "modules/interrupts/button_interrupt.h"

/* NETWORK INCLUDES */
//JC fix: include simplelink.h
#include "simplelink.h"

#include "modules/networking/tcp_network.h"
#include "network_if.h"
#include "common.h"
#include "wlan.h"
#include "utils.h"
#include "datatypes.h"

#define QUERY_REQUEST 0xDEADBEEF
#define ADD_REQUEST   0xDEADD00D

void InitializeModules() {
    UDMAInit();
    PinMuxConfig();
	I2CInit();

	InitializeInterrupts();
	InitializeMicrophone();
	InitializeDisplay();
	InitCameraComponents(640, 480);

	//Start SimpleLink in AP Mode
    long lRetVal = -1;
	lRetVal = Network_IF_InitDriver(ROLE_AP);
    if(lRetVal < 0) {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
}

// Global that is changed by button interrupt
int queryMode = 1;

void FaceRecognitionMode(void *pvParameters) {

	InitializeModules();
	int sockID = InitTcpServer(5001);

	short count = 0;
    while(1) {
    	int bufSize = 100; // big enough buffer
    	char stringBuf[bufSize];
    	char countString[10];
    	int stringLen, flag;

    	// Disable Button Interrupt
        MAP_GPIOIntDisable(INTERRUPT_BUTTON_BASE_ADDR, INTERRUPT_BUTTON_GPIO_PIN);
        MAP_IntDisable(INTERRUPT_BUTTON_GPIO_HW_INT);

        if (queryMode) {
        	flag = QUERY_REQUEST;
        	SendInt(sockID, flag);
        	TakeAndSendPicture(sockID);
        }
        else { //add mode
        	queryMode = 1; //go back to query mode after
        	flag = ADD_REQUEST;
        	SendInt(sockID, flag);
        	TakeAndSendRecording(sockID, 10000);
        	TakeAndSendPicture(sockID);
        }

        RecieveString(sockID, stringBuf, bufSize);

        ClearDisplay();
        DisplayPrintLine(stringBuf);

    	stringLen = itoa(count, countString);
    	countString[stringLen] = '\0';

    	DisplayPrintLine(countString);
    	Display();

    	// Enable Button Interrupt
    	MAP_IntEnable(INTERRUPT_BUTTON_GPIO_HW_INT);
    	MAP_GPIOIntEnable(INTERRUPT_BUTTON_BASE_ADDR, INTERRUPT_BUTTON_GPIO_PIN);

    	count++;
    	//MAP_UtilsDelay(100000);
    }
}
