#pragma once

#include <stdint.h>

class BiQuad
{

public:

	
	BiQuad(float a0, float a1, float a2, float b0, float b1, float b2);
	BiQuad(void);

	void SetParams(float a0, float a1, float a2, float b0, float b1, float b2);
	float GetOutput_DM1(float x);

	float GetOutput_TDM2(float x);

protected:

	
	float input[3];
	float output[2];

	float delaySum[2];

	//BiQuad coefficients
	float a0;
	float a1;
	float a2;

	float b0;
	float b1;
	float b2;


};