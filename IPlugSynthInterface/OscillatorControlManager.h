#pragma once

#include "IWaveformDisplay.h"
#include "IWaveformDisplayInteractive.h"
#include "Oscillator.h"

#include "IPopupMenu.h"

#include "ISelectionBox.h"
#include "IControl.h"
#include "ControlsManager.h"

class OscillatorControlManager : public ControlsManager
{

public:
	OscillatorControlManager(IPlugBase* pPlug, IGraphics* pGraphics, int32_t x, int32_t y, int32_t idx);
	virtual ~OscillatorControlManager();

	
	IPopupMenu* getLoopModeBox(void) { return loopModeMenu; }

	IWaveformDisplayInteractive* getWavDisp(void) { return wavDisp;  }
	virtual bool GetControlState(void) { return controlActiveState;	}


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

		COARSE_XOFF = 0,
		COARSE_YOFF = NORM_YOFF + 50,

		FINE_XOFF = COARSE_XOFF +  48,
		FINE_YOFF = COARSE_YOFF,
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

	enum {
		kKnobFrames = 60,
	};

	

	IWaveformDisplayInteractive* wavDisp;
	IPopupMenu* loopModeMenu;
	ISelectionBox* loopModeSelection;

	ISwitchControl* normaliseButton;
	
	IKnobMultiControl* coarseTune;
	IKnobMultiControl* fineTune;
	

};