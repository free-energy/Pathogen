

#include "ITextString.h"




ITextString::ITextString(IPlugBase *pPlug, IRECT pR, IText& ts, uint16_t strlen) :
	IControl(pPlug, pR)
{
	textStyle = new IText(ts);
	outputString = new char[strlen];
}



ITextString::~ITextString(void)
{
	delete textStyle;
	delete outputString;
}


void ITextString::SetString(char* str)
{
	strcpy(outputString, str);
	Redraw();
}

bool ITextString::Draw(IGraphics* pGraphics)
{

	//pGraphics->FillIRect(&COLOR_WHITE, &mRECT);
	pGraphics->DrawIText(textStyle, outputString, &mRECT);
	return true;
}