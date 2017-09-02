#pragma once

#include <GameEngine/UserInterface/UserInterface.h>

URHO3D_EVENT(EVENT_DATA_SHOW_MAIN_MENU_UI, SHOW_MAIN_MENU_UI)
{

}

class MainMenuView;
class CornersLogic;

class MainMenuUI : public BaseUI
{
    URHO3D_OBJECT(MainMenuUI, BaseUI);

public:
    MainMenuUI(Context* context, MainMenuView * menu);

    virtual ~MainMenuUI();

    virtual bool VOnRestore();
    virtual void VOnShutdown();
    virtual bool VOnMsgProc(AppMsg message);
    virtual void VSetVisible(bool visible);

	 void SetStartButtonText(String text);

    // Helper functions
    void CreateMainWindow();

    void StartButtonReleaseDelegate(StringHash eventType, VariantMap& eventData);
    void LobbyButtonReleaseDelegate(StringHash eventType, VariantMap& eventData);
    void OptionsButtonReleaseDelegate(StringHash eventType, VariantMap& eventData);
    void CreditsButtonReleaseDelegate(StringHash eventType, VariantMap& eventData);
    void QuitButtonReleaseDelegate(StringHash eventType, VariantMap& eventData);
	void ClientFinishGameDelegate(StringHash eventType, VariantMap& eventData);
	void ClientTryPlayAgainGameDelegate(StringHash eventType, VariantMap& eventData);

protected:
	 MainMenuView* m_pMainMenuView;
     SharedPtr<Window> m_pMainWindow;
	 SharedPtr<UIElement> m_pBackground;
	 SharedPtr<UIElement> m_pPattern;

	 bool m_bIsGameStarted;
};
