#include "CornersStd.h"

#include <GameEngine/Events/Client/ClientEvent.h>
#include <GameEngine/GameLogic/BaseGameLogic.h>

#include <GameEngine/UserInterface/LayoutUtility.h>
#include <UserInterface/ScreenElements/MainMenu/MainMenuUI.h>
#include <GameEngine/Events/GameEngineEvents.h>

#include "GameLogic/CornersLogic.h"
#include "GameView/CornersHumanView.h"

#include "GameUI.h"

GameUI::GameUI(Context* context, CornersHumanView* cornersHumanView) : BaseUI(context),
																	   m_pCornersHumanView(cornersHumanView)

{
    SubscribeToEvent(EVENT_DATA_ENVIRONMENT_LOADED, URHO3D_HANDLER(GameUI, EnvironmentLoadedDelegate));
}


void GameUI::VOnShutdown()
{
	VSetVisible(false);
}


bool GameUI::VOnRestore()
{
    if (!m_bIsInitialized)
    {
        CreateGameWindow();
        m_bIsInitialized = true;
    }

    return true;
}

void GameUI::VSetVisible(bool visible)
{
	m_bIsVisible = visible;

	if (m_pGameWindow)
	{
		m_pGameWindow->SetVisible(m_bIsVisible);
	}
}


bool GameUI::VOnMsgProc(AppMsg message)
{

    return false;
}


void GameUI::CreateGameWindow()
{
	if(!m_pGameWindow)
	{
		// Generate Layout
		String Layout = String(ScreenPath[LayoutUtility::GetLayout()]);

		// Generate Path
		String xmlLayout = String("UI/Layout/")+Layout+String("/Game.xml");

		UI* pUI = GetSubsystem<UI>();
		XMLFile* file = g_pApp->GetResCache()->GetResource<XMLFile>(xmlLayout);

		m_pGameWindow = StaticCast<Window>(pUI->LoadLayout(file));
		UIElement* windowBG = m_pGameWindow->GetChild("Credits_BG", true);

		m_pGameWindow->SetAlignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_TOP);

		unsigned int diffX = m_pGameWindow->GetWidth() - windowBG->GetWidth();
		unsigned int diffY = m_pGameWindow->GetHeight() - windowBG->GetHeight();

		unsigned int displayWidth = g_pApp->GetGraphics()->GetWidth();
		unsigned int displayHeight = g_pApp->GetGraphics()->GetHeight();

		unsigned int windowWidth = displayWidth / 2.0f + displayWidth / 3.0f;
		unsigned int windowHeight = displayHeight / 5.0f;

		m_pGameWindow->SetWidth(windowWidth);
		m_pGameWindow->SetHeight(windowHeight);

		if (windowBG)
		{
			windowBG->SetWidth(windowWidth - diffX);
			windowBG->SetHeight(windowHeight - diffY);
		}

        pUI->GetRoot()->AddChild(m_pGameWindow);

		m_pGameWindow->SetVisible(false);

		m_pFirstPlayerUsernameText = static_cast<Text*>(m_pGameWindow->GetChild(String("Username_Player_1_Value"), true));
		m_pFirstPlayerWinsText = static_cast<Text*>(m_pGameWindow->GetChild(String("Wins_Player_1_Value"), true));
		m_pFirstPlayerLosesText = static_cast<Text*>(m_pGameWindow->GetChild(String("Loses_Player_1_Value"), true));

		m_pSecondPlayerUsernameText = static_cast<Text*>(m_pGameWindow->GetChild(String("Username_Player_2_Value"), true));
		m_pSecondPlayerWinsText = static_cast<Text*>(m_pGameWindow->GetChild(String("Wins_Player_2_Value"), true));
		m_pSecondPlayerLosesText = static_cast<Text*>(m_pGameWindow->GetChild(String("Loses_Player_2_Value"), true));

		UIElement* turnHistoryButton = m_pGameWindow->GetChild("Turn_History_Button", true);
		turnHistoryButton->SetWidth(windowWidth / 2.0f - 4.0f);
		SubscribeToEvent(turnHistoryButton, E_RELEASED, URHO3D_HANDLER(GameUI, HistoryButtonReleaseDelegate));

		UIElement* backButton = m_pGameWindow->GetChild("Menu_Button", true);
		backButton->SetWidth(windowWidth / 2.0f - 4.0f);
		SubscribeToEvent(backButton, E_RELEASED, URHO3D_HANDLER(GameUI, MenuButtonReleaseDelegate));

		SubscribeToEvent(EVENT_DATA_CLIENT_FINISH_GAME, URHO3D_HANDLER(GameUI, ClientFinishGameDelegate));
	}
}

void GameUI::HistoryButtonReleaseDelegate(StringHash eventType, VariantMap& eventData)
{
	if(m_pGameWindow)
	{
		VSetVisible(false);
		SharedPtr<IScreenElement> historyWindow = m_pCornersHumanView->GetHistoryUI();
		historyWindow->VSetVisible(true);
	}
}

void GameUI::MenuButtonReleaseDelegate(StringHash eventType, VariantMap& eventData)
{
	if(m_pGameWindow)
	{
		VSetVisible(false);
		SharedPtr<IScreenElement> currentPlayerWindow = m_pCornersHumanView->GetCurrentPlayerUI();
		currentPlayerWindow->VSetVisible(false);

        SendEvent(EVENT_DATA_SHOW_MAIN_MENU_UI);
	}
}

void GameUI::SetPlayersData()
{
	CornersLogic* pCornersLogic = static_cast<CornersLogic*>(g_pApp->GetGameLogic());
	const PlayerData& firstPlayer = pCornersLogic->GetFirstPlayer();
	const PlayerData& secondPlayer = pCornersLogic->GetSecondPlayer();

	m_pFirstPlayerUsernameText->SetText(firstPlayer.m_Username);
	m_pFirstPlayerWinsText->SetText(String(firstPlayer.m_Wins));
	m_pFirstPlayerLosesText->SetText(String(firstPlayer.m_Loses));

	m_pSecondPlayerUsernameText->SetText(secondPlayer.m_Username);
	m_pSecondPlayerWinsText->SetText(String(secondPlayer.m_Wins));
	m_pSecondPlayerLosesText->SetText(String(secondPlayer.m_Loses));
}

void GameUI::ClientFinishGameDelegate(StringHash eventType, VariantMap& eventData)
{
	CornersLogic* pCornersLogic = static_cast<CornersLogic*>(g_pApp->GetGameLogic());
	const PlayerData& firstPlayer = pCornersLogic->GetFirstPlayer();
	m_pFirstPlayerWinsText->SetText(String(firstPlayer.m_Wins));
	m_pFirstPlayerLosesText->SetText(String(firstPlayer.m_Loses));

	const PlayerData& secondPlayer = pCornersLogic->GetSecondPlayer();
	m_pSecondPlayerWinsText->SetText(String(secondPlayer.m_Wins));
	m_pSecondPlayerLosesText->SetText(String(secondPlayer.m_Loses));
}


void GameUI::EnvironmentLoadedDelegate(StringHash eventType, VariantMap& eventData)
{
    SetPlayersData();
}


