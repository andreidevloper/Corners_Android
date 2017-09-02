#include "CornersStd.h"

#include <GameEngine/UserInterface/LayoutUtility.h>
#include <GameEngine/Events/GameEngineEvents.h>

#include "GameView/MainMenuView.h"
#include "OptionsUI.h"

OptionsWindowUI::OptionsWindowUI(Context* context, MainMenuView* menu) : BaseUI(context),
                                                                         m_pOptionsWindow(nullptr),
                                                                         m_pMainMenuView(menu)
{

}

void OptionsWindowUI::VOnShutdown()
{
	VSetVisible(false);
}

bool OptionsWindowUI::VOnRestore()
{
    if (!m_bIsInitialized)
    {
        CreateOptionsWindow();
        m_bIsInitialized = true;
    }

    return true;
}

void OptionsWindowUI::VSetVisible(bool visible)
{
	m_bIsVisible = visible;

	if(m_pOptionsWindow.NotNull())
	{
        m_pOptionsWindow->SetVisible(m_bIsVisible);
        m_pOptionsWindow->SetPriority(2);
	}
}


bool OptionsWindowUI::VOnMsgProc(AppMsg message)
{

    return false;
}


void OptionsWindowUI::CreateOptionsWindow()
{
	// Generate Layout
	String Layout = String(ScreenPath[LayoutUtility::GetLayout()]);

	// Generate Path
    String xmlLayout = String("UI/Layout/")+Layout+String("/Options.xml");

	UI* ui = GetSubsystem<UI>();
	XMLFile* file = g_pApp->GetResCache()->GetResource<XMLFile>(xmlLayout);
	m_pOptionsWindow = StaticCast<Window>(ui->LoadLayout(file));
	UIElement* windowBG = m_pOptionsWindow->GetChild("Credits_BG", true);

	unsigned int diffX = m_pOptionsWindow->GetWidth() - windowBG->GetWidth();
	unsigned int diffY = m_pOptionsWindow->GetHeight() - windowBG->GetHeight();

	unsigned int displayWidth = g_pApp->GetGraphics()->GetWidth();
	unsigned int displayHeight = g_pApp->GetGraphics()->GetHeight();

	unsigned int windowWidth = displayWidth / 2.0f + displayWidth / 3.0f;
	unsigned int windowHeight = displayHeight / 2.0f + displayHeight / 6.0f;

    m_pOptionsWindow->SetWidth(windowWidth);
    m_pOptionsWindow->SetHeight(windowHeight);

	if(windowBG)
	{
		windowBG->SetWidth(windowWidth - diffX);
		windowBG->SetHeight(windowHeight - diffY);
	}

	UIElement* backButton = m_pOptionsWindow->GetChild("Back_Button", true);
	backButton->SetWidth(windowWidth);
	SubscribeToEvent(backButton, E_RELEASED, URHO3D_HANDLER(OptionsWindowUI, BackButtonReleaseDelegate));

	m_pAudioSlider = (Slider*)m_pOptionsWindow->GetChild("Audio_Slider", true);

	m_pAudioSlider->SetValue(g_pApp->GetGameOptions()->GetSoundOptions().m_MasterVolume * 100.0f);

	ui->GetRoot()->AddChild(m_pOptionsWindow);

	m_pOptionsWindow->SetVisible(false);

	SubscribeToEvent(m_pAudioSlider, E_SLIDERCHANGED, URHO3D_HANDLER(OptionsWindowUI, SliderChangedDelegate));
}

void OptionsWindowUI::BackButtonReleaseDelegate(StringHash eventType, VariantMap& eventData)
{
	if(m_pOptionsWindow)
	{
		VSetVisible(false);
		SharedPtr<IScreenElement> mainWindow = m_pMainMenuView->GetMainUI();
		mainWindow->VSetVisible(true);
	}
}

void OptionsWindowUI::SliderChangedDelegate(StringHash eventType, VariantMap& eventData)
{
	float value = eventData[SliderChanged::P_VALUE].GetFloat();
	g_pApp->GetGameOptions()->GetSoundOptions().m_MasterVolume = value * 100.0f;
	SendEvent(EVENT_DATA_MASTER_VOLUME);
}