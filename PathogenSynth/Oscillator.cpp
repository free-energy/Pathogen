#include "Oscillator.h"
#include <math.h>


Oscillator::Oscillator(Wavetable* wt) : wt(wt)
{	
	masterTune = 440;
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
		LoopPoint[LOOP] = (LoopPoint[END] - LoopPoint[START]) / 2;
		smoothLoopPoints(RISING);
	}
}

double Oscillator::getPhaseInc(double pitch)
{
	uint32_t sampleRate = wt->getSampleRate();
	uint8_t originalMIDIKey = wt->getOriginalMIDIKey();

	double frequency = (double)masterTune * pow(2, ((pitch - 69.) / 12.));
	double originalFreq = (double)masterTune * pow(2, (( (double)originalMIDIKey - 69.) / 12.));

	double freqRatio = frequency / originalFreq;

	return freqRatio;
}


void Oscillator::resetInterpolator(float ph, float phInc)
{

	int32_t integer = (int32_t)phInc;
	float fraction = (phInc - integer);
	float invFraction;

	if (fraction == 0.0)
	{
		invFraction = (1.) * RESAMPLE_FACTOR;
		upsampleFactor = (integer * invFraction);
		downsampleFactor = (invFraction);
	}
	else
	{
		invFraction = (1. / fraction) * RESAMPLE_FACTOR;
		upsampleFactor = ((integer * invFraction) + 256);
		downsampleFactor = (invFraction);
	}
	
	errorAcc = 0;



}

//Need to fix the ping pong mode for this. Waveforms still sound bad
void Oscillator::updateInterpolation(float lastPhase, float newPhase)
{


}

double Oscillator::getInterpolatedSample(uint8_t ch)
{
	int32_t integer = (int32_t)phase;
	float fraction = (phase - integer);
	int32_t sign = (phaseIncrement >= 0.) ? 1 : -1;

	float invFraction = RESAMPLE_FACTOR;
	int32_t sample1ph = (int32_t)(phase);
	int32_t sample2ph = (int32_t)(sample1ph + sign);

	if (fraction != 0.0)
	{
		fraction = fraction * sign;
		//sample2ph = (int32_t)(sample1ph + sign);
		
		invFraction = (1. / fraction) * RESAMPLE_FACTOR;
	}

	float s1 = getSample(ch, sample1ph);
	float s2 = getSample(ch, sample2ph);

	float diff = s2 - s1;
	float deltaSample = (diff * RESAMPLE_FACTOR) / invFraction;

	float output = s1 + (deltaSample);

	if (ch == RIGHT_CHANNEL)
	{
		//phase = phase + phaseIncrement;
	}


	return output;


}

//Pitch in MIDI note
void Oscillator::trigger(double pitch, uint8_t velocity)
{
	if (velocity)
	{
		retrigFlag = 1;
		phase = (float)LoopPoint[START];
		if (loopMode == REVERSE)
		{
			phase = (float)LoopPoint[END] - 1;
		}

		phaseIncrement = (float)getPhaseInc(pitch);
		resetInterpolator(phase, phaseIncrement);

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

//Obtains RAW samples
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

//Takes into account the accumulator and interpolator
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

	return getInterpolatedSample(chIndex);
}

void Oscillator::updatePhase(void)
{

	float oldPhase = phase;


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

		default:
			break;
	}

}