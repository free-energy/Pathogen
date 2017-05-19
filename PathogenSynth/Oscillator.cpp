#include "Oscillator.h"
#include <math.h>


Oscillator::Oscillator(Wavetable* wt) : wt(wt)
{	
	updateWavetable(wt);

}


Oscillator::~Oscillator()
{
}

uint32_t Oscillator::getLoopPoint(uint8_t index)
{
	return LoopPoint[index];
}

void Oscillator::updateLoopPoints(uint32_t start, uint32_t loop, uint32_t end)
{
	LoopPoint[START] = start;
	LoopPoint[LOOP] = loop;
	LoopPoint[END] = end;

}

void Oscillator::updateWavetable(Wavetable* wave)
{
	this->wt = wave;
	updateWavetable();
}


void Oscillator::updateWavetable(void)
{
	phaseIncrement = 1;
	phase = 0;
	LoopPoint[START] = 0;

	LoopPoint[LOOP] = 0;
	LoopPoint[END] = 0;

	loopMode = ONE_SHOT_PING_PONG;
	loopPhase = INC;

	retrigFlag = 0;
	triggerHandled = 0;

	if (wt)
	{
		LoopPoint[END] = wt->getFrameCount();
		LoopPoint[LOOP] = (LoopPoint[END] - 20000);
		smoothLoopPoints(RISING);
	}
}

void Oscillator::trigger(uint8_t velocity)
{
	if (velocity)
	{
		retrigFlag = 1;
		phase = LoopPoint[START];
		if (loopMode == REVERSE)
		{
			phase = LoopPoint[END] - 1;
		}
		loopPhase = INC;
	}
}

//Find the first zero crossing between start and end in the direction of DIR
uint32_t Oscillator::FindZeroCrossing(uint8_t dir, uint32_t start, uint32_t end)
{
	double lastSample = getSample(LEFT_CHANNEL, start);
	double curSample = getSample(LEFT_CHANNEL, start);

	for (uint32_t i = start; i < end; ++i)
	{
		lastSample = curSample;
		curSample = getSample(LEFT_CHANNEL, i);

		if ((dir == RISING) &&
			(curSample < 0.0) &&
			(lastSample >= 0.0) &&
			(curSample != lastSample))
		{
			return i;
		}

		if ((dir == FALLING) &&
			(curSample >= 0.0) &&
			(lastSample < 0.0) &&
			(curSample != lastSample))
		{
			return i;
		}
	}
	return 0;
}


bool Oscillator::isAtZeroCrossing(uint32_t sampleIdx)
{
	double lastSample = getSample(LEFT_CHANNEL, sampleIdx+1);
	double curSample = getSample(LEFT_CHANNEL, sampleIdx-1);

	if ((lastSample == 0.0) || (curSample == 0.0))
	{
		return true;
	}

	if ((lastSample >= 0.0) && (curSample <= 0.0))
	{
		return true;
	}

	if ((lastSample <= 0.0) && (curSample >= 0.0))
	{
		return true;
	}


	return false;

}

//Find the zero crossings
uint8_t Oscillator::smoothLoopPoints(uint8_t dir)
{
	const uint32_t SEEK_LIMIT = 256;
	uint32_t tempLoopPoint[2] = { 0,0 };

	if (!isAtZeroCrossing(LoopPoint[LOOP]))
	{
		tempLoopPoint[0] = FindZeroCrossing(dir, LoopPoint[LOOP] - SEEK_LIMIT, LoopPoint[LOOP] + SEEK_LIMIT);
		if (tempLoopPoint[0] != 0)
		{
			LoopPoint[LOOP] = tempLoopPoint[0];
		}
	}

	if (!isAtZeroCrossing(LoopPoint[END]))
	{
		tempLoopPoint[1] = FindZeroCrossing(dir, LoopPoint[END] - SEEK_LIMIT, LoopPoint[END]);
		if (tempLoopPoint[1] != 0)
		{
			LoopPoint[END] = tempLoopPoint[1];
		}
	}


	return 0;

}


double Oscillator::getSample(uint8_t chIndex, uint32_t ph)
{
	if (ph >= wt->getFrameCount())
	{
		return 0.0;
	}

	double* samples = wt->getLeftSamples();
	if (chIndex == Oscillator::RIGHT_CHANNEL)
	{
		samples = wt->getRightSamples();
	}

	if (samples)
	{
		return samples[ph];
	}

	return 0.0;
}

double Oscillator::getSample(uint8_t chIndex)
{
	if (loopPhase == ONESHOT_FINISHED)
	{
		return 0.0;
	}

	if (phase >= wt->getFrameCount())
	{
		return 0.0;
	}

	double* samples = wt->getLeftSamples();
	if (chIndex == Oscillator::RIGHT_CHANNEL)
	{
		samples = wt->getRightSamples();
	}

	if (samples)
	{
		if (loopPhase == DEC)
		{
			return -samples[phase];
		}

		return samples[phase];
	}

	return 0.0;
}

void Oscillator::updatePhase(void)
{
	

	switch (loopMode)
	{
		case FORWARD:
		{
			phase += phaseIncrement;
			if (phase >= LoopPoint[END])
			{
				phase = (phase - LoopPoint[END]) + LoopPoint[START];
			}
		}
		break;

		case REVERSE:
		{
			phase -= phaseIncrement;
			//For reverse
			if (phase < LoopPoint[START])
			{
				phase = phase + LoopPoint[END] - LoopPoint[START];
			}
		}
		break;

		case PING_PONG:
		{
			if (loopPhase == INC)
			{
				phase += phaseIncrement;
				while (phase >= LoopPoint[END])
				{
					phase = phase - phaseIncrement;
					loopPhase = DEC;
				}
			}
			else if (loopPhase == DEC)
			{
				phase -= phaseIncrement;
				while (phase < LoopPoint[START])
				{
					loopPhase = INC;
					phase += phaseIncrement;
				}
			}
		}
		break;

		case ONE_SHOT:
		{
			phase += phaseIncrement;
			if (phase >= LoopPoint[END])
			{
				loopPhase = ONESHOT_FINISHED;
				phase = LoopPoint[START];
			}
		}
		break;

		case ONE_SHOT_FORWARD:
		{
			phase += phaseIncrement;
			if (phase >= LoopPoint[END])
			{
				phase = (phase - LoopPoint[END]) + LoopPoint[LOOP];
			}
		}
		break;

		case ONE_SHOT_PING_PONG:
		{
			if (loopPhase == INC)
			{
				phase += phaseIncrement;
				while (phase >= LoopPoint[END])
				{
					phase = phase - phaseIncrement;
					loopPhase = DEC;
				}
			}
			else if (loopPhase == DEC)
			{
				phase -= phaseIncrement;
				while (phase < LoopPoint[LOOP])
				{
					loopPhase = INC;
					phase += phaseIncrement;
				}
			}
		}
		break;



	}


}