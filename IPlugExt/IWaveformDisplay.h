#pragma once
#include "IControl.h"
#include <stdint.h>
#include "Wavetable.h"

// Fills a rectangle with a colour
class IWaveformDisplay : public IControl
{
public:
	IWaveformDisplay(IPlugBase *pPlug, IRECT pR, int paramIdx, const IColor* pColor);

	virtual ~IWaveformDisplay();

	virtual bool Draw(IGraphics* pGraphics);

	void setWaveformPoints(Wavetable* wt);


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

	int32_t DispStartFrame;
	int32_t DispEndFrame;

	uint32_t sampleRate;
	
	uint32_t numSamples;
	IColor mColor;

};



