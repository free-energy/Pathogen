#pragma once

#include <vector>
#include <stdint.h>
#include "IGraphics.h"
#include "IPlugBase.h"
#include "IControl.h"

class ControlsManager
{

public:
	ControlsManager(IPlugBase *pPlug, IGraphics* pGraphics, int32_t x, int32_t y, int32_t idx) : pPlug(pPlug), x(x), y(y), startParamIdx(idx) {};
	virtual ~ControlsManager() {};

	virtual void AttachControls(IGraphics* pGraphics);
	
	//Performs a redraw of controls
	virtual void Update(void);
	
	virtual void ShowControls(void);
	virtual void HideControls(void);
	virtual void SetControls(bool val);

	virtual bool GetControlState(void) { return controlActiveState; }

	int32_t getX(void) { return x; }
	int32_t getY(void) { return y; }

protected:

	IPlugBase *pPlug;

	int32_t x;
	int32_t y;

	int32_t startParamIdx;
	bool controlActiveState;


	void AddControl(IControl* c);
	
	
	std::vector<IControl*> vControls;


};