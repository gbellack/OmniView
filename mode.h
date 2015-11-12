#ifndef MODE_H
#define MODE_H

/* EFFECTS: Initializes the separate modules */
extern void InitializeModules();

/* freeRTOS tasks */
extern void FaceRecognitionMode(void *pvParameters);

extern void NameRecordingMode(void *pvParameters);


#endif
