#include "CornersStd.h"

#include <GameEngine/GameLogic/BaseGameLogic.h>
#include <GameEngine/Events/Client/ClientEvent.h>
#include <GameEngine/UserInterface/UserInterface.h>
#include <GameEngine/Events/GameEngineEvents.h>

#include "GameLogic/CornersLogic.h"
#include "CurrentPlayerUI.h"


CurrentPlayerUI::CurrentPlayerUI(Context* context, CornersHumanView* cornersHumanView) : BaseUI(context),
                                                                                         m_pCornersHumanView(cornersHumanView),
                                                                                         m_pFirstPlayerText(nullptr),
                                                                                         m_pSecondPlayerText(nullptr)
{
    SubscribeToEvent(EVENT_DATA_ENVIRONMENT_LOADED, URHO3D_HANDLER(CurrentPlayerUI, EnvironmentLoadedDelegate));
}

void CurrentPlayerUI::VOnShutdown()
{
	VSetVisible(false);
}

bool CurrentPlayerUI::VOnRestore()
{
    if (!m_bIsInitialized)
    {
        CreateCurrentPlayerWindow();
        m_bIsInitialized = true;
    }

    return true;
}

void CurrentPlayerUI::VSetVisible(bool visible)
{
    m_bIsVisible = visible;

    if(m_pCurrentPlayerWindow.NotNull())
    {
        m_pCurrentPlayerWindow->SetVisible(m_bIsVisible);
    }
}

void CurrentPlayerUI::CreateCurrentPlayerWindow()
{
    if(!m_pCurrentPlayerWindow)
    {
		// Generate Layout
		String Layout = String(ScreenPath[LayoutUtility::GetLayout()]);

		// Generate Path
		String xmlLayout = String("UI/Layout/")+Layout+String("/Current_Player.xml");

		UI* ui = GetSubsystem<UI>();
		XMLFile* file = g_pApp->GetResCache()->GetResource<XMLFile>(xmlLayout);

        m_pCurrentPlayerWindow = StaticCast<Window>(ui->LoadLayout(file));
        UIElement* windowBG = m_pCurrentPlayerWindow->GetChild("Credits_BG", true);

        unsigned int diffX = m_pCurrentPlayerWindow->GetWidth() - windowBG->GetWidth();
        unsigned int diffY = m_pCurrentPlayerWindow->GetHeight() - windowBG->GetHeight();

        unsigned int displayWidth = g_pApp->GetGraphics()->GetWidth();
        unsigned int displayHeight = g_pApp->GetGraphics()->GetHeight();

        m_pCurrentPlayerWindow->SetVisible(false);

        SubscribeToEvent(EVENT_DATA_CLIENT_FINISH_TURN, URHO3D_HANDLER(CurrentPlayerUI, ClientFinishTurnDelegate));

        ui->GetRoot()->AddChild(m_pCurrentPlayerWindow);
    }
}


void CurrentPlayerUI::SetPlayerNames()
{
	CornersLogic* pCornersLogic = static_cast<CornersLogic*>(g_pApp->GetGameLogic());
	if(pCornersLogic)
	{
		String firstPlayerName = pCornersLogic->GetFirstPlayer().m_Username;
		String secondPlayerName = pCornersLogic->GetSecondPlayer().m_Username;

		m_pFirstPlayerText = static_cast<Text*>(m_pCurrentPlayerWindow->GetChild("Username_Player_1_Value", true));
		m_pSecondPlayerText = static_cast<Text*>(m_pCurrentPlayerWindow->GetChild("Username_Player_2_Value", true));

		m_pFirstPlayerText->SetText(firstPlayerName);
		m_pSecondPlayerText->SetText(secondPlayerName);

		// First always has first turn
		m_pSecondPlayerText->SetVisible(false);
		m_pSecondPlayerText->SetEnabled(false);
	}
}

void CurrentPlayerUI::ClientFinishTurnDelegate(StringHash eventType, VariantMap& eventData)
{
	PlayerType currentPlayer = (PlayerType)eventData[CLIENT_FINISH_TURN::P_PLAYER_TYPE].GetInt();
	if(currentPlayer == FIRST_PLAYER)
	{
		m_pFirstPlayerText->SetVisible(true);
		m_pFirstPlayerText->SetEnabled(true);
		m_pSecondPlayerText->SetVisible(false);
		m_pSecondPlayerText->SetEnabled(false);
	}
	else if(currentPlayer == SECOND_PLAYER)
	{
		m_pSecondPlayerText->SetVisible(true);
		m_pSecondPlayerText->SetEnabled(true);
		m_pFirstPlayerText->SetVisible(false);
		m_pFirstPlayerText->SetEnabled(false);
	}
}

void CurrentPlayerUI::EnvironmentLoadedDelegate(StringHash eventType, VariantMap& eventData)
{
    SetPlayerNames();
}