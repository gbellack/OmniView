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
#define MILS_DELAY(x) (80000*x/6)

//GLOBALS
const int DEBUG = 1;
int TCP_ERR_COUNT = 0;
int queryMode = 1; // Global that is changed by button interrupt
//const int CAM_I2C_SLAVE_ADDR = 0x90 >> 1;  //small camera
const int CAM_I2C_SLAVE_ADDR = 0xBA >> 1;  //big camera

void InitializeModules() {
    UDMAInit();
    PinMuxConfig();

    //for the PCB WITH the display soldered on, gpio9 is the camera standby line
    //for the PCB WITHOUT the display soldered on, gpio9 is the camera reset line
    GPIOPinWrite(GPIOA1_BASE, 0x02, 0x00); // comment out this line if using pcb without display soldered on

    I2CInit();

	// Initialize camera controller, gets the processor stuff going
	CamControllerInit();

	InitializeInterrupts();
	InitializeMicrophone();
	InitializeDisplay();

	if(DEBUG) {
		ClearPrintDisplayLine("display init");
	}

	InitCameraComponents(640, 480);

	if(DEBUG) {
		ClearPrintDisplayLine("camera init");
	}

	//Start SimpleLink in AP Mode
    long lRetVal = -1;
	lRetVal = Network_IF_InitDriver(ROLE_AP);
    if(lRetVal < 0) {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
}

void FaceRecognitionMode(void *pvParameters) {
	int sockID, stringLen, flag;
	int count = 0;
	char countString[10]; // big enough buffer
	int bufSize = 100; // big enough buffer
	char crntDisplay[bufSize];

	InitializeModules();

	ClearPrintDisplayLine("ready for server to connect");
	sockID = InitTcpServer(5001);

    while(1) {
    	char stringBuf[bufSize];

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

    	if(TCP_ERR_COUNT >= 10) {
    		ClearPrintDisplayLine("ready for server to connect");
    		sockID = InitTcpServer(5001);
    	}

    	// Enable Button Interrupt
    	MAP_IntEnable(INTERRUPT_BUTTON_GPIO_HW_INT);
    	MAP_GPIOIntEnable(INTERRUPT_BUTTON_BASE_ADDR, INTERRUPT_BUTTON_GPIO_PIN);

    	count++;
    	//MAP_UtilsDelay(100000);
    }
}
