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

/* NETWORK INCLUDES */
//JC fix: include simplelink.h
#include "simplelink.h"

#include "modules/networking/tcp_network.h"
#include "network_if.h"
#include "common.h"
#include "wlan.h"
#include "utils.h"
#include "datatypes.h"

void InitializeModules() {

    UDMAInit();
    PinMuxConfig();
	I2CInit();


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

void FaceRecognitionMode(void *pvParameters) {

	InitializeModules();
	int sockID = InitTcpServer(5001);

	short count = 0;
    while(1) {

    	int bufSize = 100; // big enough buffer
    	char stringBuf[bufSize];
    	char countString[10];
    	int stringLen;

    	TakeAndSendPicture(sockID);
    	RecieveString(sockID, stringBuf, bufSize);

    	ClearDisplay();
    	DisplayPrintLine(stringBuf);

    	stringLen = itoa(count, countString);
    	countString[stringLen] = '\0';

    	DisplayPrintLine(countString);
    	Display();

    	count++;
    	//MAP_UtilsDelay(100000);
    }
}

void NameRecordingMode(void *pvParameters) {

	for (;;) {

	}
}
