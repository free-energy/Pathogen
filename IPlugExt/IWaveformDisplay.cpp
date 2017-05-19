#include "IControl.h"
#include "IWaveformDisplay.h"

#include <math.h>
#include <stdlib.h>


uint8_t IWaveformDisplay::GetClosestLoopPoint(int x)
{
	int32_t distanceTo[3];
	uint8_t minDistance;

	x = x - mRECT.L;

	uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
	int32_t selectedSample = (x * waveformStepSize) + DispStartFrame;

	int32_t startFrameX = 0;
	int32_t loopFrameX = mRECT.W() / 2;
	int32_t endFrameX = mRECT.W();

	distanceTo[START_POINT] = abs(selectedSample - LoopPoint[START_POINT]);
	distanceTo[LOOP_POINT] = abs(selectedSample - LoopPoint[LOOP_POINT]);
	distanceTo[END_POINT] = abs(selectedSample - LoopPoint[END_POINT]);

	minDistance = START_POINT;

	if (distanceTo[LOOP_POINT] < distanceTo[minDistance])
	{
		minDistance = LOOP_POINT;
	}

	if (distanceTo[END_POINT] <= distanceTo[minDistance])
	{
		minDistance = END_POINT;
	}

	return minDistance;
}

void IWaveformDisplay::OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod) 
{
	if (pMod->L == true && (pMod->C == true) )
	{
		dragX = x;
		dragY = y;

		isMagnifyMode = true;
		this->SetDirty();
	}
	else if (pMod->L == true)
	{
		uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
		LoopPoint[selectedLoopPoint] = ((x - mRECT.L) * waveformStepSize) + DispStartFrame;

		isSetLoopPointMode = true;

		this->SetDirty();
	}
}


void IWaveformDisplay::OnMouseDown(int x, int y, IMouseMod* pMod)
{
	if (pMod->L == true && (pMod->C == true))
	{
		startDragX = x;
		startDragY = y;
	}
	else if (pMod->R == true)
	{
		this->SetDirty();
		DispStartFrame = 0;
		DispEndFrame = numSamples;
	}
	else if (pMod->L == true)
	{
		uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
		selectedLoopPoint = GetClosestLoopPoint(x);



	}
}


void IWaveformDisplay::OnMouseWheel(int x, int y, IMouseMod* pMod, int d)
{
	const uint32_t ZOOM_SPEED = 2;

	//Do zoom
	if (pMod->C == true)
	{
		uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
		uint32_t currentStepWindow = mRECT.W() * waveformStepSize;

		//Anchor start or endpoint depending on where the cursor is
		//Anchor End Point, variable Start
		if (x >= (mRECT.L + (mRECT.W() / 2)) )
		{
			DispStartFrame = DispEndFrame - ( (waveformStepSize - (d * ZOOM_SPEED) ) * mRECT.W());
			DispStartFrame = minmax(DispStartFrame, 0, DispEndFrame - mRECT.W() );
		}
		//Variable End Point, Anchor Start
		else
		{
			DispEndFrame = DispStartFrame + ( (waveformStepSize - (d * ZOOM_SPEED)) * mRECT.W());
			DispEndFrame = minmax(DispEndFrame, DispStartFrame + mRECT.W(), numSamples);
		}
		this->SetDirty();
	}
	else
	{
		uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
		int32_t windowSize = DispEndFrame - DispStartFrame;
		int32_t newStartPoint = DispStartFrame + (d * waveformStepSize * (mRECT.W() / 20));

		if ((newStartPoint + windowSize) < numSamples)
		{
			DispStartFrame = minmax(newStartPoint, 0, DispEndFrame);
			DispEndFrame = DispStartFrame + windowSize;

			DispEndFrame = minmax(DispEndFrame, DispStartFrame, numSamples);

			this->SetDirty();
		}
	}


}

void IWaveformDisplay::OnMouseUp(int x, int y, IMouseMod* pMod)
{
	if (isMagnifyMode == true)
	{
		isMagnifyMode = false;
		this->SetDirty();

		uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
		
		startDragX = minmax(startDragX, mRECT.L, mRECT.R);
		dragX = minmax(dragX, mRECT.L, mRECT.R);

		int startX = min(startDragX, dragX);
		startX -= mRECT.L;

		int endX = max(startDragX, dragX);
		endX -= mRECT.L;

		int32_t initialStartFrame = DispStartFrame;

		DispStartFrame = (waveformStepSize * startX) + initialStartFrame;
		DispEndFrame = (waveformStepSize * endX ) + initialStartFrame;

		DispEndFrame = minmax(DispEndFrame, 0, numSamples);
		DispStartFrame = minmax(DispStartFrame, 0, DispEndFrame);
		
		if ((DispEndFrame - DispStartFrame) < mRECT.W())
		{
			if (DispStartFrame == 0)
			{
				DispEndFrame = DispStartFrame + mRECT.W();
			}
			else if (DispEndFrame == numSamples)
			{
				DispStartFrame = DispEndFrame - mRECT.W();
			}
			else //Take mid point
			{
				int32_t midX = (endX + startX) / 2;
				int32_t dispMidFrame = (midX * waveformStepSize) + initialStartFrame;
				DispStartFrame = dispMidFrame - ((waveformStepSize * mRECT.W()) / 2);
				DispEndFrame = dispMidFrame + ((waveformStepSize * mRECT.W()) / 2);
			}
		}
	}

	if (isSetLoopPointMode)
	{
		isSetLoopPointMode = false;
		this->SetDirty();
	}
}


void IWaveformDisplay::DrawWaveform(IGraphics* pGraphics, double* buf, const IColor* colour)
{
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

	if (isMagnifyMode)
	{
		pGraphics->DrawVerticalLine(&COLOR_GREEN, startDragX, mRECT.B, mRECT.T);
		pGraphics->DrawVerticalLine(&COLOR_GREEN, dragX, mRECT.B, mRECT.T);
	}


	if (currentSample >= DispStartFrame && currentSample <= DispEndFrame)
	{
		uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
		uint32_t linePos = mRECT.L + ( (currentSample - DispStartFrame) / waveformStepSize);
		pGraphics->DrawVerticalLine(&COLOR_YELLOW, linePos, mRECT.B - 1, mRECT.T);
	}


	for (uint8_t i = START_POINT; i < NUM_LOOP_POINTS; ++i)
	{
		IColor loopCursorColour(255, 255, 127, 0);

		if (isSetLoopPointMode && (i == selectedLoopPoint))
		{
			loopCursorColour = IColor(255, 255, 255, 255);
		}
		else
		{
			loopCursorColour = IColor(255, 255, 127, 0);
		}

		if (LoopPoint[i] >= DispStartFrame && currentSample <= DispEndFrame)
		{
			uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
			uint32_t linePos = mRECT.L + ((LoopPoint[i] - DispStartFrame) / waveformStepSize);
			pGraphics->DrawVerticalLine(&loopCursorColour, linePos, mRECT.B - 1, mRECT.T);
		}
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
	DispStartFrame = 0;  
	DispEndFrame = wt->getFrameCount() - 1;
	numSamples = wt->getFrameCount() - 1;

	LoopPoint[START_POINT] = 0;
	LoopPoint[LOOP_POINT] = numSamples / 2;
	LoopPoint[END_POINT] = numSamples;

	SetDirty();


}


