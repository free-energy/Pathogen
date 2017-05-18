#pragma once

#include <stdint.h>
#include <stdlib.h>


class Wavetable
{

public:
	Wavetable();
	virtual ~Wavetable();

	void importWave(void* buf, uint8_t format, uint8_t chCount, uint32_t frameCount);
	void setSampleRate(uint32_t sampleRate) { SampleRate = sampleRate; }

	double* getLeftSamples(void) { return LeftSamples;  }
	double* getRightSamples(void) { return RightSamples; }

	uint32_t getFrameCount(void) { return FrameCount; }

protected:

	/* Convert it to float */
	void importWave(float* buf, uint8_t chCount, uint32_t frameCount);

	/* Convert it to float */
	void importWave(int16_t* buf, uint8_t chCount, uint32_t frameCount);


	double* LeftSamples;
	double* RightSamples;
	uint32_t	  FrameCount;

	//MIDI note
	uint8_t originalMIDIKey;

	uint32_t SampleRate;

};

