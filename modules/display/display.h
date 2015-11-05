// This file will contain the display interface for OmniView.
#ifndef DISPLAY_H
#define DISPLAY_H

#include "display_driver.h"

/* NAME PARAMETERS */
#define MAX_NAMES_ALLOCATED 5
#define MAX_NAME_LENGTH 15
#define NAME_TEXT_SIZE 1

#define TEXT_COLOR WHITE

/* EFFECTS: Displays the name on the screen. */
extern void DisplayName(const char *firstName, const char *lastName);

extern void DisplayTask(void *pvParameters);

#endif
