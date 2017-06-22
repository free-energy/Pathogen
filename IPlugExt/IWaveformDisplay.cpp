#include "IControl.h"
#include "IWaveformDisplay.h"

#include <math.h>
#include <stdlib.h>

IWaveformDisplay::IWaveformDisplay(IPlugBase *pPlug, IRECT pR, int paramIdx, const IColor* pColor)
	: IControl(pPlug, pR, paramIdx), mColor(*pColor) {
	numSamples = 0;
	samplePointsLeft = 0;
	samplePointsRight = 0;
	DispStartFrame = 0;
	DispEndFrame = 0;


}	

IWaveformDisplay::~IWaveformDisplay()
{


}

void IWaveformDisplay::DrawWaveform(IGraphics* pGraphics, Samp_t* buf, const IColor* colour)
{
	if (buf == NULL)
	{
		return;
	}


	uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
	int32_t pointY = (mRECT.B - (mRECT.H() / 2)) - (int)(buf[0] * mRECT.H());
	int32_t pointX = mRECT.L;

	int32_t lastX = mRECT.L;
	int32_t lastY = pointY;

	for (uint32_t i = 0; i < mRECT.W(); i++)
	{
		pointX = mRECT.L + i;
		pointY = (mRECT.B - (mRECT.H() / 2)) - (int)((buf[waveformStepSize*i + DispStartFrame] * mRECT.H()) / 2);
		pGraphics->DrawLine(colour, lastX, lastY, pointX, pointY, &IChannelBlend(IChannelBlend::kBlendNone), true);
		lastX = pointX;
		lastY = pointY;
	}
}

bool IWaveformDisplay::Draw(IGraphics* pGraphics)
{
	pGraphics->FillIRect(&mColor, &mRECT, &mBlend);

	pGraphics->DrawHorizontalLine(&COLOR_GRAY, mRECT.T + (mRECT.H() / 2), mRECT.L, mRECT.R);

	DrawWaveform(pGraphics, samplePointsLeft, &COLOR_WHITE);
	DrawWaveform(pGraphics, samplePointsRight, &COLOR_RED);
	return true;
}


void IWaveformDisplay::setWaveformPoints(Wavetable* wt)
{
	samplePointsLeft = wt->getLeftSamples();
	samplePointsRight = wt->getRightSamples();
	DispStartFrame = 0;  
	DispEndFrame = wt->getFrameCount() - 1;
	numSamples = wt->getFrameCount() - 1;

	sampleRate = wt->getSampleRate();

	SetDirty();
	Redraw();

}


