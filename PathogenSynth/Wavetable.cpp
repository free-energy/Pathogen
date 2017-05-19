#include "Wavetable.h"
#include <limits.h>


Wavetable::Wavetable()
{
	LeftSamples = NULL;
	RightSamples = NULL;
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

void Wavetable::importWave(void* buf, uint8_t format, uint8_t chCount, uint32_t frameCount)
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