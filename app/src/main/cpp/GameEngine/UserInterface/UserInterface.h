#pragma once

#include "LayoutUtility.h"

// Base user interface element class
// Derive it when need to create GUI element on the screen
// For e.x. load Urho3D UI layout .xml file, create event handlers for GUI elements,
// And adopt created GUI element for respective screen resolution
class BaseUI : public IScreenElement
{
	URHO3D_OBJECT(BaseUI, IScreenElement);

public:
	BaseUI(Context* context); 

	// Return true when UI element is clicked, dragged, moved, etc.
	virtual bool VOnMsgProc(AppMsg message) { return false; }

	// Application is not active (e.x. win app is minimized or Android app is paused)
	virtual bool VOnLostDevice() { return true; }

	// Update UI logic
	virtual void VOnUpdate(float timeStep) { }
	virtual void VOnShutdown() { }

	// Hide/Show UI element
	virtual bool VIsVisible() const;
	virtual void VSetVisible(bool visible);

	// When need to show respective UI element in front of
	virtual int VGetZOrder() const { return 1; }
	virtual void VSetZOrder(int const zOrder) { }

protected:
	bool m_bIsVisible;
	bool m_bIsInitialized;
};
