#pragma once

#include <stdint.h>


class PathogenWavetable
{
protected:
	const double* wt;
	uint32_t	  wtLength;
};


class PathogenOsc
{
public:
	PathogenOsc();
	virtual ~PathogenOsc();


protected:
	PathogenWavetable* wt;


	
	int32_t phaseIncrement;
	
	int32_t loopIn;
	int32_t loopOut;


};

