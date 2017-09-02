#pragma once

#include <GameEngine/UserInterface/UserInterface.h>

class MainMenuView;

class OptionsWindowUI : public BaseUI
{
    URHO3D_OBJECT(OptionsWindowUI, BaseUI);

public:
    OptionsWindowUI(Context* context, MainMenuView * menu);


    virtual bool VOnRestore();
    virtual void VOnShutdown();
    virtual bool VOnMsgProc(AppMsg message);
    virtual void VSetVisible(bool visible);

    // Helper functions
    void CreateOptionsWindow();

    void BackButtonReleaseDelegate(StringHash eventType, VariantMap& eventData);
	void SliderChangedDelegate(StringHash eventType, VariantMap& eventData);

protected:
	 MainMenuView* m_pMainMenuView;
     SharedPtr<Window> m_pOptionsWindow;
	 Slider* m_pAudioSlider;
};
