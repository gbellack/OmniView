/* This file contains the timer interrupt interface
 * Created on: November 22, 2015
 * Zheng Hao Tan
 */

#ifndef TIMER_INTERRUPT_H
#define TIMER_INTERRUPT_H

#include <stdbool.h>
#include <stdint.h>
#define TIME_IN_MSECS 100
#define SAMPLE_RATE 10000
#define MIC_SAMPLE_SIZE 2

/* EFFECTS: Initializes Timer A and enables it */
extern bool RecordingDone;
extern void TimerConfigNStart(uint32_t sampleRequest, char *buf);
extern uint32_t CurrentSamples;
/* EFFECTS: Deinitializes Timer A and unregisters it */
extern void TimerDeinitStop();

/* EFFECTS: Timer Interrupt handler, which toggles the RED led */
extern void TimerPeriodicIntHandler(void);

#endif
