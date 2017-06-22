#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "Types.h"

class Wavetable
{

public:
	Wavetable();
	virtual ~Wavetable();

	void importWave(void* buf, uint8_t format, uint8_t chCount, uint32_t frameCount, uint32_t sampleRate);
	void setSampleRate(uint32_t sampleRate) { SampleRate = sampleRate; }
	uint32_t getSampleRate(void) { return SampleRate; }
	
	uint8_t getOriginalMIDIKey(void) { return originalMIDIKey; }

	Samp_t getMaxAmplitude(void);

	Samp_t* getLeftSamples(void) { return LeftSamples;  }
	Samp_t* getRightSamples(void) { return RightSamples; }

	uint32_t getFrameCount(void) { return FrameCount; }

	void Normalise(void);
	void DeNormalise(void);


protected:

	void freeMemory(void);

	/* Convert it to Samp_t */
	void importWave(Samp_t* buf, uint8_t chCount, uint32_t frameCount);

	/* Convert it to Samp_t */
	void importWave(int16_t* buf, uint8_t chCount, uint32_t frameCount);

	/* The amount each sample is divided by to obtain */
	Samp_t normaliseFactor;

	Samp_t* LeftSamples;
	Samp_t* RightSamples;
	uint32_t	  FrameCount;

	//MIDI note
	uint8_t originalMIDIKey;

	uint32_t SampleRate;

};

