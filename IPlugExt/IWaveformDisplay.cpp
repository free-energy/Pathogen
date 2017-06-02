#include "IControl.h"
#include "IWaveformDisplay.h"

#include <math.h>
#include <stdlib.h>

IWaveformDisplay::IWaveformDisplay(IPlugBase *pPlug, IRECT pR, int paramIdx, const IColor* pColor, int startLoopIdx, int LoopIdx, int endLoopIdx)
	: IControl(pPlug, pR, paramIdx), mColor(*pColor) {
	numSamples = 0;
	samplePointsLeft = 0;
	samplePointsRight = 0;
	DispStartFrame = 0;
	DispEndFrame = 0;
	isMagnifyMode = false;
	isSetLoopPointMode = false;
	currentSample = 0;
	mouseOverSample = 0;

	selectedLoopPoint = 0;

	LoopPoint[START_POINT] = 0;
	LoopPoint[LOOP_POINT] = 0;
	LoopPoint[END_POINT] = 0;

	LoopCtrl[START_POINT] = new IInvisibleSwitchControl(pPlug, pR, startLoopIdx);
	LoopCtrl[LOOP_POINT] = new IInvisibleSwitchControl(pPlug, pR, LoopIdx);
	LoopCtrl[END_POINT] = new IInvisibleSwitchControl(pPlug, pR, endLoopIdx);

	IRECT detailsPos = IRECT(pR.L + pR.W() + 10, pR.T, pR.R + pR.W() + 10, pR.B);
	detailsText = new IText(12, &COLOR_WHITE, 0, IText::kStyleNormal, IText::kAlignNear);

	MouseOverDetailsString = new char[50];
	strcpy(MouseOverDetailsString, "");
	MouseOverDetails = new ITextControl(pPlug, detailsPos, detailsText, MouseOverDetailsString);

}	

IWaveformDisplay::~IWaveformDisplay()
{
	delete LoopCtrl[START_POINT];
	delete LoopCtrl[LOOP_POINT];
	delete LoopCtrl[END_POINT];

	/* Exception occurs when trying to delete MouseOverDetails.
	delete [] MouseOverDetailsString;

	delete MouseOverDetails;*/
}

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
		isSetLoopPointMode = true;

		uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();

		setLoopPoint(selectedLoopPoint, ((x - mRECT.L) * waveformStepSize) + DispStartFrame);

		LoopCtrl[selectedLoopPoint]->SetDirty();
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
		int32_t newStartPoint = DispStartFrame - (d * waveformStepSize * (mRECT.W() / 20));

		if ((newStartPoint + windowSize) < numSamples)
		{
			DispStartFrame = minmax(newStartPoint, 0, DispEndFrame);
			DispEndFrame = DispStartFrame + windowSize;

			DispEndFrame = minmax(DispEndFrame, DispStartFrame, numSamples);

			this->SetDirty();
		}
	}
}




void IWaveformDisplay::OnMouseOver(int x, int y, IMouseMod* pMod)
{
	uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
	int cursorSample = x;
	cursorSample -= mRECT.L;
	cursorSample = (cursorSample * waveformStepSize) + DispStartFrame;


	sprintf(MouseOverDetailsString, "Sample: %u / %u\nTime: ", cursorSample , y);
	MouseOverDetails->SetTextFromPlug(MouseOverDetailsString);
	MouseOverDetails->SetDirty();


	mouseOverSample = cursorSample;


	//GetGUI()->DrawPoint(&COLOR_GREEN, x, y, &IChannelBlend(IChannelBlend::kBlendNone), true);
	//MouseOverDetails->Draw(MouseOverDetails->GetGUI());
	this->SetDirty();
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
			else if (DispEndFrame >= numSamples)
			{
				DispStartFrame = DispEndFrame - mRECT.W();
			}
			else //Take mid point
			{
				int32_t midX = (endX + startX) / 2;
				int32_t dispMidFrame = (midX * waveformStepSize) + initialStartFrame;
				DispStartFrame = dispMidFrame - (mRECT.W() / 2);
				DispEndFrame = dispMidFrame + (mRECT.W() / 2);

				DispEndFrame = minmax(DispEndFrame, 0, numSamples);
				DispStartFrame = minmax(DispStartFrame, 0, DispEndFrame);
			}
		}
	}

	if (isSetLoopPointMode)
	{
		for (uint8_t i = START_POINT; i < NUM_LOOP_POINTS; ++i)
		{
			LoopPoint[i] = minmax(LoopPoint[i], 0, numSamples);
		}

		isSetLoopPointMode = false;

		LoopCtrl[START_POINT]->SetDirty();

		this->SetDirty();
	}
}

int32_t  IWaveformDisplay::GetSampleAsYCoOrds(double* buf, uint32_t sampleIdx)
{
	uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
	int32_t pointY = (mRECT.B - (mRECT.H() / 2)) - (int)((buf[sampleIdx] * mRECT.H()) / 2);
	
	return pointY;
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


	if (mouseOverSample >= DispStartFrame && mouseOverSample <= DispEndFrame)
	{
		uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
		uint32_t linePos = mRECT.L + ((mouseOverSample - DispStartFrame) / waveformStepSize);
		pGraphics->FillCircle(&COLOR_GREEN, linePos, GetSampleAsYCoOrds(samplePointsLeft, mouseOverSample), 2);
		pGraphics->FillCircle(&COLOR_BLUE, linePos, GetSampleAsYCoOrds(samplePointsRight, mouseOverSample), 2);
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

		if ( (LoopPoint[i] >= DispStartFrame) && (LoopPoint[i] <= DispEndFrame) )
		{
			uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / (mRECT.W() - 1);
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

void IWaveformDisplay::setLoopPoint(uint8_t index, uint32_t val)
{
	LoopPoint[index] = val;

	LoopPoint[START_POINT] = minmax(LoopPoint[START_POINT], 0, LoopPoint[LOOP_POINT]);
	LoopPoint[LOOP_POINT] = minmax(LoopPoint[LOOP_POINT], LoopPoint[START_POINT], LoopPoint[END_POINT]);
	LoopPoint[END_POINT] = minmax(LoopPoint[END_POINT], LoopPoint[LOOP_POINT], numSamples);

	SetDirty();


}


int32_t IWaveformDisplay::getLoopPoint(/* eLoopPoints */ uint8_t index)
{
	if (index < NUM_LOOP_POINTS)
	{
		return LoopPoint[index];
	}
	return 0;
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


