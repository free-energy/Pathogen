#include "Wavetable.h"
#include <math.h>
#include <limits.h>


Wavetable::Wavetable()
{
	LeftSamples = NULL;
	RightSamples = NULL;

	originalMIDIKey = 52; //E4;
	//originalMIDIKey = 60; //C5;

	normaliseFactor = 1.0;

	FrameCount = 0;
	SampleRate = 0;
}


Wavetable::~Wavetable()
{
	freeMemory();
}


void Wavetable::freeMemory(void)
{
	if ((RightSamples != NULL) && (RightSamples != LeftSamples))
	{
		free(RightSamples);
		RightSamples = NULL;
	}

	if (LeftSamples != NULL)
	{
		free(LeftSamples);
		LeftSamples = NULL;
	}
}

void Wavetable::importWave(void* buf, uint8_t format, uint8_t chCount, uint32_t frameCount, uint32_t sampleRate)
{
	freeMemory();

	if (format == 1)
	{
		importWave((int16_t*)buf, chCount, frameCount);
	}
	else if ( format == 3 )
	{
		importWave((Samp_t*)buf, chCount, frameCount);
	}

	FrameCount = frameCount;
	SampleRate = sampleRate;
	normaliseFactor = 1.0;
}

void Wavetable::importWave(Samp_t* buf, uint8_t chCount, uint32_t frameCount)
{
	LeftSamples = (Samp_t*)malloc( frameCount * sizeof(Samp_t) );
	if (chCount == 2)
	{
		RightSamples = (Samp_t*)malloc(frameCount * sizeof(Samp_t));
	}
	else
	{
		RightSamples = LeftSamples;
	}

	for (uint32_t i = 0; i < frameCount; ++i)
	{
		LeftSamples[i] = (Samp_t)buf[chCount*i];
		if (chCount == 2)
		{
			RightSamples[i] = (Samp_t)buf[(chCount * i) + 1];
		}
	}

}

void Wavetable::importWave(int16_t* buf, uint8_t chCount, uint32_t frameCount)
{
	LeftSamples = (Samp_t*)malloc(frameCount * sizeof(Samp_t));
	if (chCount == 2)
	{
		RightSamples = (Samp_t*)malloc(frameCount * sizeof(Samp_t));
	}
	else
	{
		RightSamples = LeftSamples;
	}

	for (uint32_t i = 0; i < frameCount; ++i)
	{
		LeftSamples[i] = (Samp_t) (Samp_t(buf[chCount*i]) / (Samp_t)(32768));
		if (chCount == 2)
		{
			RightSamples[i] = (Samp_t)(Samp_t(buf[(chCount*i) + 1]) / (Samp_t)(32768));
		}
	}

}



Samp_t Wavetable::getMaxAmplitude(void)
{
	Samp_t maxSample = 0.;
	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		if (fabsf(LeftSamples[i]) > maxSample)
		{
			maxSample = fabsf(LeftSamples[i]);
		}
		
		if (fabsf(RightSamples[i]) > maxSample)
		{
			maxSample = fabsf(RightSamples[i]);
		}
	}

	return maxSample;
}


void Wavetable::Normalise(void)
{
	if (LeftSamples == NULL)
	{
		return;
	}

	normaliseFactor = getMaxAmplitude();
	if (normaliseFactor == 0.)
	{
		normaliseFactor = 1.0;
	}

	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		LeftSamples[i] = LeftSamples[i] / normaliseFactor;
		RightSamples[i] = RightSamples[i] / normaliseFactor;
	}

}


void Wavetable::DeNormalise(void)
{
	if (normaliseFactor == 0.)
	{
		normaliseFactor = 1.0;
	}

	if (LeftSamples == NULL)
	{
		return;
	}

	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		LeftSamples[i] = LeftSamples[i] * normaliseFactor;
		RightSamples[i] = RightSamples[i] * normaliseFactor;
	}

	normaliseFactor = 1.0;
}