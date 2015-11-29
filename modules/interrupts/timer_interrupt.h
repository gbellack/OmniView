/* This file contains the timer interrupt interface
 * Created on: November 22, 2015
 * Zheng Hao Tan
 */

#ifndef TIMER_INTERRUPT_H
#define TIMER_INTERRUPT_H

#define TIME_IN_MSECS 100

/* EFFECTS: Initializes Timer A and enables it */
extern void TimerConfigNStart();

/* EFFECTS: Deinitializes Timer A and unregisters it */
extern void TimerDeinitStop();

/* EFFECTS: Timer Interrupt handler, which toggles the RED led */
extern void TimerPeriodicIntHandler(void);

#endif
