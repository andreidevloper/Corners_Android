#pragma once

#include <GameEngine/UserInterface/UserInterface.h>

class CornersHumanView;

class CurrentPlayerUI : public BaseUI
{
    URHO3D_OBJECT(CurrentPlayerUI, BaseUI);

public:
    CurrentPlayerUI(Context* context, CornersHumanView * menu);

    virtual bool VOnRestore();
    virtual void VOnShutdown();
    virtual void VSetVisible(bool visible);

    // Helper functions
    void CreateCurrentPlayerWindow();
    void ClientFinishTurnDelegate(StringHash eventType, VariantMap& eventData);
	void SetPlayerNames();

protected:
	void EnvironmentLoadedDelegate(StringHash eventType, VariantMap& eventData);

protected:
	CornersHumanView* m_pCornersHumanView;
    SharedPtr<Window> m_pCurrentPlayerWindow;
	Text* m_pFirstPlayerText;
	Text* m_pSecondPlayerText;
};
