#pragma once

#include <GameEngine/UserInterface/UserInterface.h>

class CornersHumanView;
class MainMenuView;

URHO3D_EVENT(EVENT_DATA_SHOW_GAME_UI, SHOW_GAME_UI)
{

}

class GameUI : public BaseUI
{
    URHO3D_OBJECT(GameUI, BaseUI);

public:
    GameUI(Context* context, CornersHumanView* cornersHumanView);
  
    virtual bool VOnRestore();
    virtual void VOnShutdown();
    virtual bool VOnMsgProc(AppMsg message);
    virtual void VSetVisible(bool visible);

    // Helper functions
    void CreateGameWindow();

    void SetPlayersData();

private:
    void HistoryButtonReleaseDelegate(StringHash eventType, VariantMap& eventData);
    void MenuButtonReleaseDelegate(StringHash eventType, VariantMap& eventData);
	void ClientFinishGameDelegate(StringHash eventType, VariantMap& eventData);
    void EnvironmentLoadedDelegate(StringHash eventType, VariantMap& eventData);

protected:
    CornersHumanView* m_pCornersHumanView;
    SharedPtr<Window> m_pGameWindow;

    Text* m_pFirstPlayerUsernameText;
    Text* m_pFirstPlayerWinsText;
    Text* m_pFirstPlayerLosesText;

    Text* m_pSecondPlayerUsernameText;
    Text* m_pSecondPlayerWinsText;
    Text* m_pSecondPlayerLosesText;
};
