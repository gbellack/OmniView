/* This file contains the modes for the interrupt handler function.
 * When the button gets pressed, this part will change OmniView into
 * face recognition mode or name recording mode. */
#include "mode.h"

/* DISPLAY INCLUDES */
#include "modules/display/display.h"

#include "modules/mcu/mcu.h"

/* MICROPHONE INCLUDES */
#include "modules/microphone/microphone.h"

/* CAMERA INCLUDES */
#include "modules/camera/httpserverapp.h"

/* TODO: Add all init functions/steps here */
void InitializeModules() {

	/* Display */
//    InitializeDisplay();

    /* Camera */

    /* Microphone */

}

void FaceRecognitionMode(void *pvParameters) {

    InitializeDisplay();
	ClearDisplay();
	SetCursor(0,0);
	DisplayName("GARRISON", "BELLACK");
	DisplayName("ZHENG HAO", "TAN");
	DisplayName("TZU-FEI", "YU");
	Display();
    UtilsDelay(8000000);

	ClearDisplay();
	SetCursor(0,0);
	DisplayName("LOREN", "WANG");
	Display();

    UtilsDelay(8000000);

	ClearDisplay();
	SetCursor(0,0);
	DisplayName("TZU-FEI", "YU");
	Display();

	HttpServerAppTask();

    for (;;) {

    }
}

void NameRecordingMode(void *pvParameters) {

	for (;;) {

	}
}
