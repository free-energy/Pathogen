#pragma once

#include "IWaveformDisplay.h"
#include "Oscillator.h"

class OscillatorManager
{

public:
	OscillatorManager(Oscillator* o, IWaveformDisplay* w) : osc(o), wavDisp(w) {};
	virtual ~OscillatorManager() {};


protected:
	IWaveformDisplay* wavDisp;
	Oscillator* osc;



};