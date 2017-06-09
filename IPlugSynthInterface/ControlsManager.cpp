


#include "ControlsManager.h"

void ControlsManager::AddControl(IControl* c)
{
	vControls.push_back(c);
}


void ControlsManager::Update(void)
{
	for (uint32_t i = 0; i < vControls.size(); ++i)
	{
		vControls[i]->Redraw();
	}


}

void ControlsManager::ShowControls(void)
{
	SetControls(true);
}


void ControlsManager::HideControls(void)
{
	SetControls(false);
}




/**
Setting val to true will activate Controls, false will disable
*/
void ControlsManager::SetControls(bool val)
{
	for (uint32_t i = 0; i < vControls.size(); ++i)
	{
		vControls[i]->Hide(!val);
	}
	controlActiveState = val;
}

void ControlsManager::AttachControls(IGraphics* pGraphics)
{

	for (uint32_t i = 0; i < vControls.size(); ++i)
	{
		pGraphics->AttachControl(vControls[i]);
	}

}
