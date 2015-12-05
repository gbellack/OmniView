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
	CamControllerInit(); //needs to be done before InitializeDisplay()
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

#define DEBUG 1
// Global that is changed by button interrupt
int queryMode = 1;

void FaceRecognitionMode(void *pvParameters) {

	InitializeModules();
	int sockID = InitTcpServer(5001);

	short count = 0;
	int bufSize = 100; // big enough buffer
	char crntDisplay[bufSize];
    while(1) {
    	char stringBuf[bufSize];
    	char countString[10];
    	int stringLen;

    	// Disable Button Interrupt
        MAP_GPIOIntDisable(INTERRUPT_BUTTON_BASE_ADDR, INTERRUPT_BUTTON_GPIO_PIN);
        MAP_IntDisable(INTERRUPT_BUTTON_GPIO_HW_INT);

        if (queryMode) {
        	SendInt(sockID, QUERY_REQUEST);
        	TakeAndSendPicture(sockID);
        }
        else { //add mode
        	SendInt(sockID, ADD_REQUEST);
        	TakeAndSendRecording(sockID, 10000);
        	TakeAndSendPicture(sockID);
        }

        RecieveString(sockID, stringBuf, bufSize);


        if(DEBUG || strcmp(crntDisplay, stringBuf) != 0) {
        	strcpy(crntDisplay, stringBuf);
        	ClearDisplay();
        	DisplayPrintLine(stringBuf);

        	if(DEBUG) {
        		stringLen = itoa(count, countString);
        		countString[stringLen] = '\0';
        		DisplayPrintLine(countString);
        	}

        	Display();
        }

    	if(!queryMode) {
    		queryMode = 1; //go back to query mode after this iteration
    		MAP_UtilsDelay(1000000); //delay for effect
    	}

    	// Enable Button Interrupt
    	MAP_IntEnable(INTERRUPT_BUTTON_GPIO_HW_INT);
    	MAP_GPIOIntEnable(INTERRUPT_BUTTON_BASE_ADDR, INTERRUPT_BUTTON_GPIO_PIN);

    	count++;
    	//MAP_UtilsDelay(100000);
    }
}
