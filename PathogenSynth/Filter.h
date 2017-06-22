#pragma once

#include <stdint.h>
#include "BiQuad.h"
#include "Types.h"

class Filter
{

public:

	enum eFilterModes
	{
		LPF,
		BPF,
		HPF,
		NOTCH,
		NUM_MODES,
	};

	Filter(Samp_t Fs);
	virtual ~Filter(void);

	void Set_f0(Samp_t f0) { SetParams(f0, Q, Mode); }
	void Set_Q(Samp_t q) { SetParams(this->f0, q, Mode); }
	void Set_Mode(int mode) { SetParams(this->f0, Q, mode);}

	void SetParams(Samp_t f0, Samp_t q, int mode);

	Samp_t* kernel;

	Samp_t convolute(Samp_t X, Samp_t* H);

	virtual Samp_t processSample(Samp_t s, int ch);



protected:
	Samp_t Q;
	Samp_t f0;
	Samp_t Fs;

	Samp_t w0;
	Samp_t alpha;
	Samp_t a0;

	int Mode;

	BiQuad* bqL;
	BiQuad* bqR;


};