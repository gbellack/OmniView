// TI CC3200 MCU Interface.

// This file will contain the interface for the TI CC3200 MCU chip
#ifndef MCU_H
#define MCU_H

// Enum of all connected modules.
typedef enum {
	CAMERA,
	DISPLAY,
	MICROPHONE,
	WIRELESS
} mcu_connected_modules;

// http://processors.wiki.ti.com/index.php/CC32xx_Sensor_Profile_Application
// EFFECTS: Puts the processor in hibernate mode.
extern void enterHibernateMode();

// EFFECTS: Exit hibernate mode.
extern void exitHibernateMode();

extern void enterDeepSleepMode();

extern void exitDeepSleepMode();

#endif
