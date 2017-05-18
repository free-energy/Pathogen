#include "Oscillator.h"



Oscillator::Oscillator(Wavetable* wt) : wt(wt)
{
	phaseIncrement = 1;
	phase = 0;

	loopOut = wt->getFrameCount();
	loopIn = 0;
}


Oscillator::~Oscillator()
{
}


void Oscillator::process(double* left, double* right)
{
	double* lSamples = wt->getLeftSamples();
	double* rSamples = wt->getRightSamples();

	*left =  lSamples[phase];
	*right = rSamples[phase];

	phase += phaseIncrement;
	if (phase >= loopOut)
	{
		phase = loopIn;
	}

	if (phase < loopIn)
	{
		phase = loopOut;
	}

}