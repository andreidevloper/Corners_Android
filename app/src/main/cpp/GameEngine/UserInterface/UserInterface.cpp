#include "GameEngineStd.h"
#include "UserInterface.h"

BaseUI::BaseUI(Context* context) : IScreenElement(context)
{
	m_bIsVisible = true;
	m_bIsInitialized = false;
}

bool BaseUI::VIsVisible() const
{
	return m_bIsVisible;
}


void BaseUI::VSetVisible(bool visible)
{ 
	m_bIsVisible = visible; 
}