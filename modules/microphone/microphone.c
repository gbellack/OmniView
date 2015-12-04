#include "microphone.h"
#include "adc.h"
#include "rom_map.h"
#include "hw_memmap.h"
#include "../interrupts/timer_interrupt.h"
#include <stdbool.h>


//this function accepts a pointer to a buffer
//and the number of seconds of recording is desired
//and fills the buffer with microphone data
//it returns the number of bytes of data stored in the buffer.
//The current system is set up to record 10000 samples per second.
//this corresponds to 20000 bytes per second
// some common values of numSeconds
// milSec = 1000 -> 20000 bytes
uint32_t GetAudio(char * buf, int milSec){

	uint32_t numSamples = milSec * (SAMPLE_RATE / 1000);
	TimerConfigNStart(numSamples, buf);
	//TimerConfigNStart(1000, buf);

	while(!RecordingDone){
	}
	TimerDeinitStop();
	return MIC_SAMPLE_SIZE*CurrentSamples;

}

//this function grabs an ADC sample
inline uint16_t GetMicSample() {
    /*
    ADC returns 32 bits
    [1:0] : Reserved
	[13:2] : ADC sample Bits
	[30:14]: time stamp per ADC sample
	[31] : Reserved
     */
    //grab only the mic value
	return ((MAP_ADCFIFORead(ADC_BASE, ADC_CH_0) >> 2) & 0xFFF);
}




//used to initialize the ADC associated with the microphone, must be called before
//you do any mic stuff
void InitializeMicrophone() {

		// // Configure ADC timer which is used to timestamp the ADC data samples
		//
		MAP_ADCTimerConfig(ADC_BASE,2^17);

		//
		// Enable ADC timer which is used to timestamp the ADC data samples
		//
		MAP_ADCTimerEnable(ADC_BASE);

	 	// Enable ADC module
		MAP_ADCEnable(ADC_BASE);

		//
		// Enable ADC channel
		//
		MAP_ADCChannelEnable(ADC_BASE, ADC_CH_0);
}
