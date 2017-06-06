


#include "OscillatorManager.h"


OscillatorManager::OscillatorManager(Oscillator* o, IWaveformDisplayInteractive* w) : osc(o), wavDisp(w)
{
	loopModeBox = new IPopupMenu();


	loopModeBox->AddItem("Forward");
	loopModeBox->AddItem("Reverse");
	loopModeBox->AddItem("Ping Pong");

	

}

OscillatorManager::~OscillatorManager(void)
{
	delete loopModeBox;

}