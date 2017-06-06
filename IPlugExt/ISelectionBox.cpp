
#include "ISelectionBox.h"


ISelectionBox::ISelectionBox(IPlugBase *pPlug, IRECT pR, IPopupMenu* menu, int paramIdx) :
	IControl(pPlug, pR, paramIdx), menu(menu)
{
	itemFont = new IText(12, &COLOR_BLACK, 0, IText::kStyleNormal, IText::kAlignNear);
	menu->SetChosenItemIdx(0);
	itemTextBuf = (char*)menu->GetItemText(menu->GetChosenItemIdx());

}


ISelectionBox::~ISelectionBox(void)
{
	delete itemFont;
	//delete itemText;
}


void ISelectionBox::OnMouseDown(int x, int y, IMouseMod* pMod)
{
	IPopupMenu* selectedMenu = mPlug->GetGUI()->CreateIPopupMenu(menu, &mRECT);
	SetDirty();
}


bool ISelectionBox::Draw(IGraphics* pGraphics)
{
	pGraphics->FillIRect(&COLOR_WHITE, &mRECT);
	pGraphics->DrawIText(itemFont, (char*)menu->GetItemText(menu->GetChosenItemIdx()), &mRECT);
	return true;
}