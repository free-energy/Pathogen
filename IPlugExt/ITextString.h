#pragma once


#include "IGraphics.h"
#include "IControl.h"
#include <stdint.h>
#include <string.h>

class ITextString : public IControl
{

public: 
	ITextString(IPlugBase *pPlug, IRECT pR, IText& ts, uint16_t strlen);
	virtual ~ITextString(void);

	void SetString(char* str);

	bool Draw(IGraphics* pGraphics);

protected:

	IText* textStyle;
	char* outputString;


};
