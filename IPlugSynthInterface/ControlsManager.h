#pragma once

#include <stdint.h>
#include "IGraphics.h"
#include "IPlugBase.h"


class ControlsManager
{

public:
	ControlsManager(IPlugBase *pPlug, IGraphics* pGraphics, int32_t x, int32_t y, int32_t idx) : pPlug(pPlug), x(x), y(y), startParamIdx(idx) {};
	virtual ~ControlsManager() {};

	virtual void AttachControls(IGraphics* pGraphics) = 0;
	
protected:

	IPlugBase *pPlug;

	int32_t x;
	int32_t y;

	int32_t startParamIdx;
	



};