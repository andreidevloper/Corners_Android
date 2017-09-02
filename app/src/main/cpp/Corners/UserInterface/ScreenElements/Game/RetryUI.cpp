#include "CornersStd.h"

#include <GameEngine/Events/Client/ClientEvent.h>
#include <GameEngine/UserInterface/LayoutUtility.h>
#include <UserInterface/ScreenElements/MainMenu/MainMenuUI.h>

#include "GameView/CornersHumanView.h"
#include "GameView/MainMenuView.h"
#include "GameLogic/CornersLogic.h"

#include "RetryUI.h"

RetryUI::RetryUI(Context* context, CornersHumanView* cornersHumanView) : BaseUI(context),
                                                                             m_pCornersHumanView(cornersHumanView),
																		     m_pFirstPlayerText(nullptr),
																		     m_pSecondPlayerText(nullptr)
{

	m_bIsDraw = false;
}


void RetryUI::VOnShutdown()
{
	VSetVisible(false);
}

bool RetryUI::VOnRestore()
{
    if (!m_bIsInitialized)
    {
        CreateRetryWindow();
        m_bIsInitialized = true;
    }

    return true;
}

void RetryUI::VSetVisible(bool visible)
{
    m_bIsVisible = visible;

    if(m_pRetryWindow.NotNull())
    {
	    m_pRetryWindow->SetVisible(m_bIsVisible);
    }
}


bool RetryUI::VOnMsgProc(AppMsg message)
{

    return false;
}


void RetryUI::CreateRetryWindow()
{
    if(!m_pRetryWindow)
    {
	     m_DrawColor = Color(1.0f, 1.0f, 0.1f, 1.0f);

		// Generate Layout
		String Layout = String(ScreenPath[LayoutUtility::GetLayout()]);

		// Generate Path
		String xmlLayout = String("UI/Layout/")+Layout+String("/Retry.xml");

		UI* pUI = GetSubsystem<UI>();
		XMLFile* file = g_pApp->GetResCache()->GetResource<XMLFile>(xmlLayout);

		m_pRetryWindow = StaticCast<Window>(pUI->LoadLayout(file));

        unsigned int windowWidth = m_pRetryWindow->GetWidth();

        m_pRetryWindow->SetVisible(false);

        m_pInfoText = (Text*)m_pRetryWindow->GetChild("Winner_Is", true);
        m_pFirstPlayerText = (Text*)m_pRetryWindow->GetChild("Winner_Username_Text_1", true);
        m_pSecondPlayerText = (Text*)m_pRetryWindow->GetChild("Winner_Username_Text_2", true);

        m_WinColor = m_pFirstPlayerText->GetColor(Corner::C_TOPRIGHT);

        UIElement* playAgainButton = m_pRetryWindow->GetChild("Play_Again_Button", true);
        playAgainButton->SetWidth(windowWidth / 2.0f - 4.0f);
        SubscribeToEvent(playAgainButton, E_RELEASED, URHO3D_HANDLER(RetryUI, PlayAgainButtonReleaseDelegate));

        UIElement* backButton = m_pRetryWindow->GetChild("Menu_Button", true);
        backButton->SetWidth(windowWidth / 2.0f - 4.0f);
        SubscribeToEvent(backButton, E_RELEASED, URHO3D_HANDLER(RetryUI, MenuButtonReleaseDelegate));

        SubscribeToEvent(EVENT_DATA_CLIENT_FINISH_GAME, URHO3D_HANDLER(RetryUI, ClientFinishGameDelegate));

        pUI->GetRoot()->AddChild(m_pRetryWindow);
    }
}

void RetryUI::PlayAgainButtonReleaseDelegate(StringHash eventType, VariantMap& eventData)
{
    if(m_pRetryWindow)
    {
        VSetVisible(false);
        SendEvent(EVENT_DATA_CLIENT_TRY_PLAY_AGAIN_GAME);
    }
}

void RetryUI::MenuButtonReleaseDelegate(StringHash eventType, VariantMap& eventData)
{
	if(m_pRetryWindow)
	{
		VSetVisible(false);
		SetEnableGamesUI(false);

        SendEvent(EVENT_DATA_SHOW_MAIN_MENU_UI);

		if(m_bIsDraw)
		{
			// Return default style
			m_pInfoText->SetText("Winner is");
			m_pFirstPlayerText->SetColor(m_WinColor);
			m_bIsDraw = false;
		}
	}
}

void RetryUI::SetEnableGamesUI(bool enable)
{
	SharedPtr<IScreenElement> gameWindow = m_pCornersHumanView->GetGameUI();
	gameWindow->VSetVisible(enable);

	SharedPtr<IScreenElement> currentPlayerWindow = m_pCornersHumanView->GetCurrentPlayerUI();
	currentPlayerWindow->VSetVisible(enable);
}

void RetryUI::ClientFinishGameDelegate(StringHash eventType, VariantMap& eventData)
{
	PlayerType winnerPlayer = (PlayerType)eventData[CLIENT_FINISH_GAME::P_PLAYER_TYPE].GetInt();
	String winnerName = eventData[CLIENT_FINISH_GAME::P_WINNER_PLAYER_NAME].GetString();
	if(m_pRetryWindow)
	{
		VSetVisible(true);

		if(winnerPlayer == FIRST_PLAYER)
		{
			m_pFirstPlayerText->SetText(winnerName);
			m_pFirstPlayerText->SetVisible(true);
			m_pFirstPlayerText->SetEnabled(true);
			m_pSecondPlayerText->SetVisible(false);
			m_pSecondPlayerText->SetEnabled(false);
		}
		else if(winnerPlayer == SECOND_PLAYER)
		{
			m_pSecondPlayerText->SetText(winnerName);
			m_pSecondPlayerText->SetVisible(true);
			m_pSecondPlayerText->SetEnabled(true);
			m_pFirstPlayerText->SetVisible(false);
			m_pFirstPlayerText->SetEnabled(false);
		}
		else if(winnerPlayer == PlayerType::INVALID_PLAYER && winnerName == "Draw")
		{
			m_pInfoText->SetText("You played a");
			m_pFirstPlayerText->SetText(winnerName);
			m_pFirstPlayerText->SetVisible(true);
			m_pFirstPlayerText->SetEnabled(true);
			m_pSecondPlayerText->SetVisible(false);
			m_pSecondPlayerText->SetEnabled(false);
			m_pFirstPlayerText->SetColor(m_DrawColor);
			m_bIsDraw = true;
		}
	}
}


