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
		importWave((float*)buf, chCount, frameCount);
	}

	FrameCount = frameCount;
	SampleRate = sampleRate;
	normaliseFactor = 1.0;
}

void Wavetable::importWave(float* buf, uint8_t chCount, uint32_t frameCount)
{
	LeftSamples = (double*)malloc( frameCount * sizeof(double) );
	if (chCount == 2)
	{
		RightSamples = (double*)malloc(frameCount * sizeof(double));
	}
	else
	{
		RightSamples = LeftSamples;
	}

	for (uint32_t i = 0; i < frameCount; ++i)
	{
		LeftSamples[i] = (double)buf[chCount*i];
		if (chCount == 2)
		{
			RightSamples[i] = (double)buf[(chCount * i) + 1];
		}
	}

}

void Wavetable::importWave(int16_t* buf, uint8_t chCount, uint32_t frameCount)
{
	LeftSamples = (double*)malloc(frameCount * sizeof(double));
	if (chCount == 2)
	{
		RightSamples = (double*)malloc(frameCount * sizeof(double));
	}
	else
	{
		RightSamples = LeftSamples;
	}

	for (uint32_t i = 0; i < frameCount; ++i)
	{
		LeftSamples[i] = (double) (double(buf[chCount*i]) / (double)(32768));
		if (chCount == 2)
		{
			RightSamples[i] = (double)(double(buf[(chCount*i) + 1]) / (double)(32768));
		}
	}

}



float Wavetable::getMaxAmplitude(void)
{
	float maxSample = 0.;
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

	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		LeftSamples[i] = LeftSamples[i] * normaliseFactor;
		RightSamples[i] = RightSamples[i] * normaliseFactor;
	}

	normaliseFactor = 1.0;
}