
#include "BiQuad.h"



BiQuad::BiQuad(void)
{
	BiQuad(0., 0., 0., 0., 0., 0.);
}

BiQuad::BiQuad(Samp_t a0, Samp_t a1, Samp_t a2, Samp_t b0, Samp_t b1, Samp_t b2) :
	a0(a0), a1(a1), a2(a2), b0(b0), b1(b1), b2(b2)
{
	input[0] = 0.;
	input[1] = 0.;
	input[2] = 0.;

	output[0] = 0.;
	output[1] = 0.;

	delaySum[0] = 0.;
	delaySum[1] = 0.;

}


void BiQuad::SetParams(Samp_t a0, Samp_t a1, Samp_t a2, Samp_t b0, Samp_t b1, Samp_t b2)
{
	this->a0 = 1;
	this->a1 = a1 / a0;
	this->a2 = a2 / a0;
	this->b0 = b0 / a0;
	this->b1 = b1 / a0;
	this->b2 = b2 / a0;


}

Samp_t BiQuad::GetOutput_TDM2(Samp_t x)
{
	Samp_t y;

	y = (b0)* x +
		delaySum[0];
	delaySum[0] = delaySum[1] + (b1 * x) - (a1 * y);
	delaySum[1] = (b2 * x) - (a2 * y);
	
	return y;
}

Samp_t BiQuad::GetOutput_DM1(Samp_t x)
{
	Samp_t y;

	y = (b0)* x +
		(b1)* input[0] +
		(b2)* input[1] -
		(a1)* output[0] -
		(a2)* output[1];


	input[1] = input[0];
	input[0] = x;

	output[1] = output[0];
	output[0] = y;


	return y;
}


