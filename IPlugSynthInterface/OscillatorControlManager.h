#pragma once

#include "IWaveformDisplay.h"
#include "IWaveformDisplayInteractive.h"
#include "Oscillator.h"

#include "IPopupMenu.h"

#include "ISelectionBox.h"

#include "ControlsManager.h"

class OscillatorControlManager : public ControlsManager
{

public:
	OscillatorControlManager(IPlugBase* pPlug, IGraphics* pGraphics, int32_t x, int32_t y, int32_t idx);
	virtual ~OscillatorControlManager();

	
	IPopupMenu* getLoopModeBox(void) { return loopModeMenu; }

	virtual void AttachControls(IGraphics* pGraphics);

	IWaveformDisplayInteractive* getWavDisp(void) { return wavDisp;  }


protected:
	
	enum {
		WAVEDISP_XOFF = 100,
		WAVEDISP_YOFF = 10,

		WAVEDISP_XLEN = 550,
		WAVEDISP_YLEN = 90,

		LOOPSEL_XOFF = 10,
		LOOPSEL_YOFF = 10,

		LOOPSEL_XLEN = 50,
		LOOPSEL_YLEN = 12,

		NORM_XOFF = LOOPSEL_XOFF,
		NORM_YOFF = LOOPSEL_YOFF + 30,
	};

	//Parameter index offsets
	enum {
		START_PT = 0,
		LOOP_PT,
		END_PT,

		LOOP_MODE,
		NORMALISE,

		COARSE_TUNE,
		FINE_TUNE,

	};

	IWaveformDisplayInteractive* wavDisp;
	IPopupMenu* loopModeMenu;
	ISelectionBox* loopModeSelection;

	ISwitchControl* normaliseButton;
	

};