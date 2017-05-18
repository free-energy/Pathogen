#pragma once

#include <stdint.h>
#include "Wavetable.h"


class Oscillator
{
public:
	Oscillator(Wavetable* wt);
	virtual ~Oscillator();

	void process(double* left, double* right);


protected:
	Wavetable* wt;

	int32_t phase;
	int32_t phaseIncrement;

	uint32_t loopIn;
	uint32_t loopOut;


};

