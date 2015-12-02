/*
 * Microphone Interface
 * This file will contain the interface for the microphone.
 * Zheng Hao Tan
 */

#ifndef MICROPHONE_H
#define MICROPHONE_H
#include <stdint.h>

extern inline uint16_t GetMicSample();
extern void InitializeMicrophone();
extern uint32_t GetAudio(char *buf, uint8_t numSeconds);
#endif
