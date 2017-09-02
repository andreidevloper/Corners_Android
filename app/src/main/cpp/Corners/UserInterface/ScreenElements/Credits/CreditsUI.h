#pragma once

#include <GameEngine/UserInterface/UserInterface.h>

class MainMenuView;
class CreditsUI : public BaseUI
{
    URHO3D_OBJECT(CreditsUI, BaseUI);

public:
    CreditsUI(Context* context, MainMenuView * menu);

    virtual bool VOnRestore();
    virtual void VOnShutdown();
    virtual bool VOnMsgProc(AppMsg message);
    virtual void VSetVisible(bool visible);
    
protected:
    // Helper functions
    void CreateCreditsWindow();
    void BackButtonReleaseDelegate(StringHash eventType, VariantMap& eventData);

protected:
    MainMenuView* m_pMainMenuView;
    SharedPtr<Window> m_pCreditsWindow;
};
