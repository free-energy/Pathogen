#pragma once
#include "IControl.h"
#include <stdint.h>
#include "Wavetable.h"

// Fills a rectangle with a colour
class IWaveformDisplay : public IControl
{
public:
	IWaveformDisplay(IPlugBase *pPlug, IRECT pR, int paramIdx, const IColor* pColor, int startLoopIdx, int LoopIdx, int endLoopIdx);

	virtual ~IWaveformDisplay();

	enum eLoopPoint
	{
		START_POINT,
		LOOP_POINT,
		END_POINT,
		NUM_LOOP_POINTS,
	};

	bool Draw(IGraphics* pGraphics);

	void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod);
	void OnMouseUp(int x, int y, IMouseMod* pMod);
	void OnMouseDown(int x, int y, IMouseMod* pMod);
	void OnMouseWheel(int x, int y, IMouseMod* pMod, int d);
	void setWaveformPoints(Wavetable* wt);


	void setCurrentSample(uint32_t curSample);

	void setLoopPoint(uint8_t index, uint32_t val);
	//void setLoopPoints(uint32_t start, uint32_t loop, uint32_t end);
	int32_t getLoopPoint(/* eLoopPoints */ uint8_t index);


	bool isSetLoopMode(void) { return isSetLoopPointMode; }

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

	uint8_t GetClosestLoopPoint(int x);

	double* samplePointsLeft;
	double* samplePointsRight;

	int32_t DispStartFrame;
	int32_t DispEndFrame;

	uint32_t currentSample;

	uint32_t numSamples;
	IColor mColor;


	//Used to determine loop points
	int32_t LoopPoint[NUM_LOOP_POINTS];

	uint8_t selectedLoopPoint; //eLoopPoint

	bool isMagnifyMode;
	bool isSetLoopPointMode;

	int startDragX;
	int startDragY;

	int dragX;
	int dragY;

	IInvisibleSwitchControl* LoopCtrl[NUM_LOOP_POINTS];


};



