#pragma once
#include "IControl.h"
#include <stdint.h>
#include "Wavetable.h"

// Fills a rectangle with a colour
class IWaveformDisplay : public IControl
{
public:
	IWaveformDisplay(IPlugBase *pPlug, IRECT pR, const IColor* pColor)
		: IControl(pPlug, pR), mColor(*pColor) {
		numSamples = 0;
		samplePointsLeft = 0;
		samplePointsRight = 0;
		startPoint = 0;
		endPoint = 0;
		isDrag = false;
		currentSample = 0;
	}

	bool Draw(IGraphics* pGraphics);

	void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod);
	void OnMouseUp(int x, int y, IMouseMod* pMod);
	void OnMouseDown(int x, int y, IMouseMod* pMod);
	void OnMouseWheel(int x, int y, IMouseMod* pMod, int d);
	void setWaveformPoints(Wavetable* wt);

	void setCurrentSample(uint32_t curSample);

protected:

	//Returns the minimum of 2 values
	int32_t min(int32_t v1, int32_t v2)
	{
		return v1 > v2 ? v2 : v1;
	}

	//Returns the maximum of 2 values
	int32_t max(int32_t v1, int32_t v2)
	{
		return v2 > v1 ? v2 : v1;
	}

	int32_t minmax(int32_t val, int32_t minVal, int32_t maxVal)
	{
		if (val < minVal)
		{
			return minVal;
		}
		if (val > maxVal)
		{
			return maxVal;
		}
		return val;
	}

	void DrawWaveform(IGraphics* pGraphics, double* buf, const IColor* colour);

	double* samplePointsLeft;
	double* samplePointsRight;

	int32_t startPoint;
	int32_t endPoint;

	uint32_t currentSample;

	uint32_t numSamples;
	IColor mColor;


	bool isDrag;

	int startDragX;
	int startDragY;

	int dragX;
	int dragY;

	

};



