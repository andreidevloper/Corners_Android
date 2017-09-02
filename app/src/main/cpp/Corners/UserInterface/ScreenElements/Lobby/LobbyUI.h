#pragma once

#include <GameEngine/UserInterface/UserInterface.h>

class MainMenuView;

class LobbyUI : public BaseUI
{
    URHO3D_OBJECT(LobbyUI, BaseUI);

public:
    LobbyUI(Context* context, MainMenuView * menu);

    virtual bool VOnRestore();
    virtual void VOnShutdown();
    virtual bool VOnMsgProc(AppMsg message);
    virtual void VSetVisible(bool visible);

    // Helper functions
    void CreateLobbyWindow();

    void BackButtonReleaseDelegate(StringHash eventType, VariantMap& eventData);
    void ClientFinishGameDelegate(StringHash eventType, VariantMap& eventData);

protected:
    MainMenuView* m_pMainMenuView;
    SharedPtr<Window> m_pLobbyWindow;

    LineEdit* m_pNameEdit;
    Text* m_pWinsText;
    Text* m_pLosesText;
    Text* m_pTotalText;
};
