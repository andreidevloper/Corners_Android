#pragma once

#include <GameEngine/UserInterface/UserInterface.h>

class CornersHumanView;
class MainMenuView;

class RetryUI : public BaseUI
{
    URHO3D_OBJECT(RetryUI, BaseUI);

public:
    RetryUI(Context* context, CornersHumanView* cornersHumanView);
   
    virtual bool VOnRestore();
    virtual void VOnShutdown();
    virtual bool VOnMsgProc(AppMsg message);
    virtual void VSetVisible(bool visible);

    // Helper functions
    void CreateRetryWindow();

    void MenuButtonReleaseDelegate(StringHash eventType, VariantMap& eventData);
    void PlayAgainButtonReleaseDelegate(StringHash eventType, VariantMap& eventData);
    void ClientFinishGameDelegate(StringHash eventType, VariantMap& eventData);


    void SetEnableGamesUI(bool enable);

protected:
     Color m_DrawColor; 
     Color m_WinColor;

     CornersHumanView* m_pCornersHumanView;
     SharedPtr<Window> m_pRetryWindow;

     Text* m_pInfoText;
	 Text* m_pFirstPlayerText;
	 Text* m_pSecondPlayerText;
	 bool m_bIsDraw;
};
