


#include "OscillatorControlManager.h"
#include "resource.h"

OscillatorControlManager::OscillatorControlManager(IPlugBase* pPlug, IGraphics* pGraphics, int32_t x, int32_t y, int32_t idx) :
	ControlsManager(pPlug, pGraphics, x,y,idx)
{


	loopModeMenu = new IPopupMenu();


	loopModeMenu->AddItem("Forward");
	loopModeMenu->AddItem("Reverse");
	loopModeMenu->AddItem("Ping Pong");

	loopModeMenu->AddItem("One Shot");
	loopModeMenu->AddItem("One Shot Loop");
	loopModeMenu->AddItem("One Shot Ping Pong");

	

	wavDisp = new IWaveformDisplayInteractive(pPlug, IRECT(x + WAVEDISP_XOFF, y + WAVEDISP_YOFF, x + WAVEDISP_XOFF + WAVEDISP_XLEN, y + WAVEDISP_YOFF + WAVEDISP_YLEN), -1, 
		&COLOR_BLACK, startParamIdx + START_PT, startParamIdx + LOOP_PT, startParamIdx + END_PT);
	

	loopModeSelection = new ISelectionBox(pPlug, IRECT(	x + LOOPSEL_XOFF, 
														y + LOOPSEL_YOFF, 
														x + LOOPSEL_XOFF + LOOPSEL_XLEN,
														y + LOOPSEL_YOFF + LOOPSEL_YLEN), loopModeMenu, startParamIdx + LOOP_MODE);

	IBitmap NormButton = pGraphics->LoadIBitmap(OSC1_NORMALISE_ID, O1_NORMALISE_FN, 2);
	normaliseButton = new ISwitchControl(pPlug, x + NORM_XOFF, y + NORM_YOFF, startParamIdx + NORMALISE, &NormButton);


	IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
	
	coarseTune = new IKnobMultiControl(pPlug, x + COARSE_XOFF, y + COARSE_YOFF, startParamIdx + COARSE_TUNE, &knob);
	fineTune = new IKnobMultiControl(pPlug, x + FINE_XOFF, y + FINE_YOFF, startParamIdx + FINE_TUNE, &knob);



	detailsText = new IText(12, &COLOR_WHITE, 0, IText::kStyleNormal, IText::kAlignCenter);

	IRECT detailsPos = IRECT(getX() + COARSE_TXT_XOFF, getY() + COARSE_TXT_YOFF, getX() + COARSE_TXT_XOFF + 1, getY() + COARSE_TXT_YOFF + 30);
	CoarseString = new ITextString(pPlug, detailsPos, *detailsText, MAX_STRING_LEN);

	detailsPos = IRECT(getX() + FINE_TXT_XOFF, getY() + FINE_TXT_YOFF, getX() + FINE_TXT_XOFF + 1, getY() + FINE_TXT_YOFF + 30);


	FineString = new ITextString(pPlug, detailsPos, *detailsText, MAX_STRING_LEN);

	AddControl(CoarseString);
	AddControl(FineString);




	controlActiveState = false;



	AddControl(wavDisp);
	AddControl(loopModeSelection);
	AddControl(coarseTune);
	AddControl(fineTune);
	AddControl(normaliseButton);
}

OscillatorControlManager::~OscillatorControlManager(void)
{
	delete loopModeMenu;
	delete wavDisp;
	delete loopModeSelection;
	delete coarseTune;
	delete fineTune;
	delete normaliseButton;


	delete detailsText;
	delete CoarseString;
	delete FineString;
}





void OscillatorControlManager::Update(void)
{
	UpdateText();
	ControlsManager::Update();
}

void OscillatorControlManager::UpdateText(void)
{
	char outputString[20];

	sprintf(&outputString[0], "Coarse Tune\n%4.2f", coarseTune->GetValue());

	CoarseString->SetString(outputString);

	sprintf(&outputString[0], "Fine Tune\n%4.2f", fineTune->GetValue());
	FineString->SetString(outputString);
}
