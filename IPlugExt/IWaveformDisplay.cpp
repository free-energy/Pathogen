#include "IControl.h"
#include "IWaveformDisplay.h"

#include <math.h>

void IWaveformDisplay::OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod) 
{
	if (pMod->L == true)
	{
		dragX = x;
		dragY = y;

		isDrag = true;
		this->SetDirty();
	}
}


void IWaveformDisplay::OnMouseDown(int x, int y, IMouseMod* pMod)
{
	if (pMod->L == true)
	{
		startDragX = x;
		startDragY = y;
	}

	if (pMod->R == true)
	{
		this->SetDirty();
		startPoint = 0;
		endPoint = numSamples;
	}
}


void IWaveformDisplay::OnMouseWheel(int x, int y, IMouseMod* pMod, int d)
{
	const uint32_t ZOOM_SPEED = 2;

	//Do zoom
	if (pMod->C == true)
	{
		uint32_t waveformStepSize = (endPoint - startPoint) / mRECT.W();
		uint32_t currentStepWindow = mRECT.W() * waveformStepSize;

		//Anchor start or endpoint depending on where the cursor is
		//Anchor End Point, variable Start
		if (x >= (mRECT.L + (mRECT.W() / 2)) )
		{
			startPoint = endPoint - ( (waveformStepSize - (d * ZOOM_SPEED) ) * mRECT.W());
			startPoint = minmax(startPoint, 0, endPoint - mRECT.W() );
		}
		//Variable End Point, Anchor Start
		else
		{
			endPoint = startPoint + ( (waveformStepSize - (d * ZOOM_SPEED)) * mRECT.W());
			endPoint = minmax(endPoint, startPoint + mRECT.W(), numSamples);
		}
		this->SetDirty();
	}
	else
	{
		uint32_t waveformStepSize = (endPoint - startPoint) / mRECT.W();
		int32_t windowSize = endPoint - startPoint;
		int32_t newStartPoint = startPoint + (d * waveformStepSize * (mRECT.W() / 20));

		if ((newStartPoint + windowSize) < numSamples)
		{
			startPoint = minmax(newStartPoint, 0, endPoint);
			endPoint = startPoint + windowSize;

			endPoint = minmax(endPoint, startPoint, numSamples);

			this->SetDirty();
		}
	}


}

void IWaveformDisplay::OnMouseUp(int x, int y, IMouseMod* pMod)
{
	if (isDrag == true)
	{
		isDrag = false;
		this->SetDirty();

		uint32_t waveformStepSize = (endPoint - startPoint) / mRECT.W();
		
		startDragX = minmax(startDragX, mRECT.L, mRECT.R);
		dragX = minmax(dragX, mRECT.L, mRECT.R);

		int startX = min(startDragX, dragX);
		startX -= mRECT.L;

		int endX = max(startDragX, dragX);
		endX -= mRECT.L;

		startPoint = waveformStepSize * startX;
		endPoint = waveformStepSize * endX;

	}
}


void IWaveformDisplay::DrawWaveform(IGraphics* pGraphics, double* buf, const IColor* colour)
{
	uint32_t waveformStepSize = (endPoint - startPoint) / mRECT.W();
	int32_t pointY = (mRECT.B - (mRECT.H() / 2)) - (int)(buf[0] * mRECT.H());
	int32_t pointX = mRECT.L;

	int32_t lastX = mRECT.L;
	int32_t lastY = pointY;

	for (uint32_t i = 0; i < mRECT.W(); i++)
	{
		pointX = mRECT.L + i;
		pointY = (mRECT.B - (mRECT.H() / 2)) - (int)((buf[waveformStepSize*i + startPoint] * mRECT.H()) / 2);
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

	if (isDrag)
	{
		pGraphics->DrawVerticalLine(&COLOR_GREEN, startDragX, mRECT.B, mRECT.T);
		pGraphics->DrawVerticalLine(&COLOR_GREEN, dragX, mRECT.B, mRECT.T);
	}


	if (currentSample >= startPoint && currentSample <= endPoint)
	{
		uint32_t waveformStepSize = (endPoint - startPoint) / mRECT.W();
		uint32_t linePos = mRECT.L + ( (currentSample - startPoint) / waveformStepSize);
		pGraphics->DrawVerticalLine(&COLOR_YELLOW, linePos, mRECT.B - 1, mRECT.T);
	}


	return true;
}





void IWaveformDisplay::setCurrentSample(uint32_t curSample)
{
	currentSample = curSample;


	SetDirty();
}


void IWaveformDisplay::setWaveformPoints(Wavetable* wt)
{
	samplePointsLeft = wt->getLeftSamples();
	samplePointsRight = wt->getRightSamples();
	startPoint = 0;  
	endPoint = wt->getFrameCount() - 1;
	numSamples = wt->getFrameCount() - 1;

	SetDirty();

}


