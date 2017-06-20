
#include "BiQuad.h"



BiQuad::BiQuad(void)
{
	BiQuad(0., 0., 0., 0., 0., 0.);
}

BiQuad::BiQuad(float a0, float a1, float a2, float b0, float b1, float b2) :
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


void BiQuad::SetParams(float a0, float a1, float a2, float b0, float b1, float b2)
{
	this->a0 = 1;
	this->a1 = a1 / a0;
	this->a2 = a2 / a0;
	this->b0 = b0 / a0;
	this->b1 = b1 / a0;
	this->b2 = b2 / a0;


}

float BiQuad::GetOutput_TDM2(float x)
{
	float y;

	y = (b0)* x +
		delaySum[0];
	delaySum[0] = delaySum[1] + (b1 * x) - (a1 * y);
	delaySum[1] = (b2 * x) - (a2 * y);
	
	return y;
}

float BiQuad::GetOutput_DM1(float x)
{
	float y;

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


