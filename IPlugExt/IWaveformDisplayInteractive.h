#pragma once
#include "IControl.h"
#include <stdint.h>
#include "Wavetable.h"
#include "IWaveformDisplay.h"
#include "Types.h"

// Fills a rectangle with a colour
class IWaveformDisplayInteractive : public IWaveformDisplay
{
public:
	IWaveformDisplayInteractive(IPlugBase *pPlug, IRECT pR, int paramIdx, const IColor* pColor, int startLoopIdx, int LoopIdx, int endLoopIdx);

	virtual ~IWaveformDisplayInteractive();

	enum eLoopPoint
	{
		START_POINT,
		LOOP_POINT,
		END_POINT,
		NUM_LOOP_POINTS,
	};

	virtual bool Draw(IGraphics* pGraphics);

	void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod);
	void OnMouseUp(int x, int y, IMouseMod* pMod);
	void OnMouseDown(int x, int y, IMouseMod* pMod);
	void OnMouseWheel(int x, int y, IMouseMod* pMod, int d);
	
	void OnMouseOver(int x, int y, IMouseMod* pMod);
	
	void setCurrentSample(uint32_t curSample);

	void setLoopPoint(uint8_t index, uint32_t val);
	void setLoopPoints(uint32_t start, uint32_t loop, uint32_t end);
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

	int32_t  GetSampleAsYCoOrds(Samp_t* buf, uint32_t sampleIdx);
	void ReDrawDetails(void);
	void ReDrawDetails(int xPos);

	uint8_t GetClosestLoopPoint(int x);


	int32_t currentSample;
	int32_t mouseOverSample;

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

	char* MouseOverDetailsString;
	IText* detailsText;

};



