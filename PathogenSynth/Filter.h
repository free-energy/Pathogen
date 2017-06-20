#pragma once

#include <stdint.h>
#include "BiQuad.h"

class Filter
{

public:

	enum eFilterModes
	{
		LPF,
		BPF,
		HPF,
	};

	Filter(float Fs);
	virtual ~Filter(void);

	void Set_f0(float f0) { SetParams(f0, Q, Mode); }
	void Set_Q(float q) { SetParams(this->f0, q, Mode); }
	void Set_Mode(int mode) { SetParams(this->f0, Q, mode);}

	void SetParams(float f0, float q, int mode);

	double* kernel;

	double convolute(double X, double* H);

	virtual float processSample(float s, int ch);



protected:
	float Q;
	float f0;
	float Fs;

	float w0;
	float alpha;
	float a0;

	int Mode;

	BiQuad* bqL;
	BiQuad* bqR;


};