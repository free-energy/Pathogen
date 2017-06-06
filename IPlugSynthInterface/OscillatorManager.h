#pragma once

#include "IWaveformDisplay.h"
#include "IWaveformDisplayInteractive.h"
#include "Oscillator.h"

#include "IPopupMenu.h"

#include "ISelectionBox.h"

class OscillatorManager
{

public:
	OscillatorManager(Oscillator* o, IWaveformDisplayInteractive* w);
	virtual ~OscillatorManager();

	
	IPopupMenu* getLoopModeBox(void) { return loopModeBox; }

protected:
	IWaveformDisplayInteractive* wavDisp;
	Oscillator* osc;

	IPopupMenu* loopModeBox;
	





};