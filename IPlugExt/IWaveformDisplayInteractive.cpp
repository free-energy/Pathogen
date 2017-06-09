#include "IControl.h"

#include "IWaveformDisplayInteractive.h"

#include <math.h>
#include <stdlib.h>

IWaveformDisplayInteractive::IWaveformDisplayInteractive(IPlugBase *pPlug, IRECT pR, int paramIdx, const IColor* pColor, int startLoopIdx, int LoopIdx, int endLoopIdx)
	: IWaveformDisplay(pPlug, pR, paramIdx, pColor) {

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

	
	detailsText = new IText(12, &COLOR_WHITE, 0, IText::kStyleNormal, IText::kAlignNear);

	MouseOverDetailsString = new char[50];
	strcpy(MouseOverDetailsString, "");
	//MouseOverDetails = new ITextControl(pPlug, detailsPos, detailsText, MouseOverDetailsString);

}	

IWaveformDisplayInteractive::~IWaveformDisplayInteractive()
{
	delete LoopCtrl[START_POINT];
	delete LoopCtrl[LOOP_POINT];
	delete LoopCtrl[END_POINT];

	/* Exception occurs when trying to delete MouseOverDetails.
	delete [] MouseOverDetailsString;

	delete MouseOverDetails;*/
}

uint8_t IWaveformDisplayInteractive::GetClosestLoopPoint(int x)
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

	if (distanceTo[LOOP_POINT] <= distanceTo[minDistance])
	{
		minDistance = LOOP_POINT;
	}

	if (distanceTo[END_POINT] <= distanceTo[minDistance])
	{
		minDistance = END_POINT;
	}

	return minDistance;
}

void IWaveformDisplayInteractive::OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod) 
{
	if (pMod->L == true && (pMod->C == true) )
	{
		dragX = x;
		dragY = y;

		isMagnifyMode = true;

		dragX = minmax(dragX, mRECT.L, mRECT.R);

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

	ReDrawDetails(x);
}


void IWaveformDisplayInteractive::OnMouseDown(int x, int y, IMouseMod* pMod)
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


void IWaveformDisplayInteractive::OnMouseWheel(int x, int y, IMouseMod* pMod, int d)
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

	ReDrawDetails(x);
}




void IWaveformDisplayInteractive::OnMouseOver(int x, int y, IMouseMod* pMod)
{
	ReDrawDetails(x);
	this->SetDirty();
}

void IWaveformDisplayInteractive::OnMouseUp(int x, int y, IMouseMod* pMod)
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
				DispStartFrame = minmax(DispStartFrame, 0, DispEndFrame - mRECT.W());
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

int32_t  IWaveformDisplayInteractive::GetSampleAsYCoOrds(double* buf, uint32_t sampleIdx)
{
	uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
	int32_t pointY = (mRECT.B - (mRECT.H() / 2)) - (int)((buf[sampleIdx] * mRECT.H()) / 2);
	
	return pointY;
}


void IWaveformDisplayInteractive::ReDrawDetails(void)
{

	if (sampleRate == 0)
	{
		return;
	}

	uint32_t secs = mouseOverSample / sampleRate;
	uint32_t millisecs = (mouseOverSample - (secs * sampleRate)) / (sampleRate / 1000);

	sprintf(MouseOverDetailsString, "Sample: %u / %u\nTime: %u:%03u s", 
		mouseOverSample, numSamples, secs, millisecs);

	IRECT detailsPos = IRECT(mRECT.L + 10, mRECT.T, mRECT.R + mRECT.W(), mRECT.B);

	GetGUI()->DrawIText(detailsText, MouseOverDetailsString, &detailsPos);

}

void IWaveformDisplayInteractive::ReDrawDetails(int xPos)
{
	uint32_t waveformStepSize = (DispEndFrame - DispStartFrame) / mRECT.W();
	int cursorSample = xPos;
	cursorSample -= mRECT.L;
	cursorSample = minmax(cursorSample, 0, mRECT.W() - 1);
	cursorSample = (cursorSample * waveformStepSize) + DispStartFrame;

	mouseOverSample = cursorSample;

	//ReDrawDetails();
	
}


bool IWaveformDisplayInteractive::Draw(IGraphics* pGraphics)
{

	IWaveformDisplay::Draw(pGraphics);

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

	ReDrawDetails();


	return true;
}





void IWaveformDisplayInteractive::setCurrentSample(uint32_t curSample)
{
	currentSample = curSample;
	SetDirty();
}

void IWaveformDisplayInteractive::setLoopPoint(uint8_t index, uint32_t val)
{
	LoopPoint[index] = val;

	LoopPoint[START_POINT] = minmax(LoopPoint[START_POINT], 0, LoopPoint[LOOP_POINT]);
	LoopPoint[LOOP_POINT] = minmax(LoopPoint[LOOP_POINT], LoopPoint[START_POINT], LoopPoint[END_POINT]);
	LoopPoint[END_POINT] = minmax(LoopPoint[END_POINT], LoopPoint[LOOP_POINT], numSamples);

	SetDirty();
}


int32_t IWaveformDisplayInteractive::getLoopPoint(/* eLoopPoints */ uint8_t index)
{
	if (index < NUM_LOOP_POINTS)
	{
		return LoopPoint[index];
	}
	return 0;
}


