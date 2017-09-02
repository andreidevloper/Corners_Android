#include "CornersStd.h"

#include <GameEngine/UserInterface/LayoutUtility.h>

#include "GameView/MainMenuView.h"
#include "CreditsUI.h"

CreditsUI::CreditsUI(Context* context, MainMenuView* menu) : BaseUI(context),
                                                                         m_pCreditsWindow(nullptr),
                                                                         m_pMainMenuView(menu)
{
}


void CreditsUI::VOnShutdown()
{
	VSetVisible(false);
}


bool CreditsUI::VOnRestore()
{
    if (!m_bIsInitialized)
    {
        CreateCreditsWindow();
        m_bIsInitialized = true;
    }

    return true;
}

void CreditsUI::VSetVisible(bool visible)
{
    m_bIsVisible = visible;

    if(m_pCreditsWindow.NotNull())
    {
	    m_pCreditsWindow->SetVisible(m_bIsVisible);
        m_pCreditsWindow->SetPriority(2);
    }
}


bool CreditsUI::VOnMsgProc(AppMsg message)
{

    return false;
}


void CreditsUI::CreateCreditsWindow()
{
    if(!m_pCreditsWindow)
    {
		// Generate Layout
		String Layout = String(ScreenPath[LayoutUtility::GetLayout()]);

		// Generate Path
		String xmlLayout = String("UI/Layout/")+Layout+String("/Credits.xml");

		XMLFile* file = g_pApp->GetResCache()->GetResource<XMLFile>(xmlLayout);

		UI* ui = GetSubsystem<UI>();
		m_pCreditsWindow = StaticCast<Window>(ui->LoadLayout(file));
		UIElement* windowBG = m_pCreditsWindow->GetChild("Credits_BG", true);

		unsigned int diffX = m_pCreditsWindow->GetWidth() - windowBG->GetWidth();
		unsigned int diffY = m_pCreditsWindow->GetHeight() - windowBG->GetHeight();

		unsigned int displayWidth = g_pApp->GetGraphics()->GetWidth();
		unsigned int displayHeight = g_pApp->GetGraphics()->GetHeight();

		unsigned int windowWidth = displayWidth / 2.0f + displayWidth / 3.0f;
		unsigned int windowHeight = displayHeight / 2.0f + displayHeight / 6.0f;

		m_pCreditsWindow->SetWidth(windowWidth);
		m_pCreditsWindow->SetHeight(windowHeight);

		m_pCreditsWindow->SetVisible(false);

		if(windowBG)
		{
			windowBG->SetWidth(windowWidth - diffX);
			windowBG->SetHeight(windowHeight - diffY);
		}

		UIElement* backButton = m_pCreditsWindow->GetChild("Back_Button", true);
		backButton->SetWidth(windowWidth);
		SubscribeToEvent(backButton, E_RELEASED, URHO3D_HANDLER(CreditsUI, BackButtonReleaseDelegate));

		ui->GetRoot()->AddChild(m_pCreditsWindow);
    }
}

void CreditsUI::BackButtonReleaseDelegate(StringHash eventType, VariantMap& eventData)
{
	if(m_pCreditsWindow)
	{
		VSetVisible(false);
		SharedPtr<IScreenElement> mainWindow = m_pMainMenuView->GetMainUI();
		mainWindow->VSetVisible(true);
	}
}


