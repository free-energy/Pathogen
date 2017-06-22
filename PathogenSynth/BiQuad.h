#pragma once

#include <stdint.h>
#include "Types.h"

class BiQuad
{

public:

	
	BiQuad(Samp_t a0, Samp_t a1, Samp_t a2, Samp_t b0, Samp_t b1, Samp_t b2);
	BiQuad(void);

	void SetParams(Samp_t a0, Samp_t a1, Samp_t a2, Samp_t b0, Samp_t b1, Samp_t b2);
	Samp_t GetOutput_DM1(Samp_t x);

	Samp_t GetOutput_TDM2(Samp_t x);

protected:

	
	Samp_t input[3];
	Samp_t output[2];

	Samp_t delaySum[2];

	//BiQuad coefficients
	Samp_t a0;
	Samp_t a1;
	Samp_t a2;

	Samp_t b0;
	Samp_t b1;
	Samp_t b2;


};