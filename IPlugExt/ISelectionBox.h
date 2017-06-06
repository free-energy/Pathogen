#pragma once

#include "IControl.h"
#include "IPopupMenu.h"

class ISelectionBox : public IControl
{
public:

	ISelectionBox(IPlugBase *pPlug, IRECT pR, IPopupMenu* menu, int paramIdx);
	virtual ~ISelectionBox(void);

	virtual bool Draw(IGraphics* pGraphics);


	virtual void OnMouseDown(int x, int y, IMouseMod* pMod);

	int getSelectedItem(void) { return menu->GetChosenItemIdx(); }


protected:
	IRECT mRect;
	IPopupMenu* menu;

	IText*		  itemFont;
	char*         itemTextBuf;

};


