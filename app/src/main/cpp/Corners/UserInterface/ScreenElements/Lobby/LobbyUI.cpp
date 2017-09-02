#include "CornersStd.h"

#include <GameEngine/Events/Database/DatabaseEvent.h>
#include <GameEngine/Events/Client/ClientEvent.h>

#include <GameEngine/UserInterface/LayoutUtility.h>

#include "GameLogic/CornersLogic.h"
#include "GameView/MainMenuView.h"

#include "LobbyUI.h"

LobbyUI::LobbyUI(Context* context, MainMenuView* menu) : BaseUI(context),
                                                                     m_pLobbyWindow(nullptr),
                                                                     m_pMainMenuView(menu)
{

}

void LobbyUI::VOnShutdown()
{
	VSetVisible(false);
}

bool LobbyUI::VOnRestore()
{
    if (!m_bIsInitialized)
    {
        CreateLobbyWindow();
        m_bIsInitialized = true;
    }

    return true;
}

void LobbyUI::VSetVisible(bool visible)
{
	if (m_pLobbyWindow)
	{
		m_bIsVisible = false;
		m_pLobbyWindow->SetVisible(visible);
        m_pLobbyWindow->SetPriority(2);
	}
}


bool LobbyUI::VOnMsgProc(AppMsg message)
{

    return false;
}


void LobbyUI::CreateLobbyWindow()
{
	if(!m_pLobbyWindow)
	{
		UI* ui = GetSubsystem<UI>();

		// Generate Layout
		String Layout = String(ScreenPath[LayoutUtility::GetLayout()]);

		// Generate Path
		String xmlLayout = String("UI/Layout/")+Layout+String("/Lobby.xml");

		XMLFile* file = g_pApp->GetResCache()->GetResource<XMLFile>(xmlLayout);
		m_pLobbyWindow = StaticCast<Window>(ui->LoadLayout(file));
		UIElement* windowBG = m_pLobbyWindow->GetChild("Credits_BG", true);

		unsigned int diffX = m_pLobbyWindow->GetWidth() - windowBG->GetWidth();
		unsigned int diffY = m_pLobbyWindow->GetHeight() - windowBG->GetHeight();

		unsigned int displayWidth = g_pApp->GetGraphics()->GetWidth();
		unsigned int displayHeight = g_pApp->GetGraphics()->GetHeight();

		unsigned int windowWidth = displayWidth / 2.0f + displayWidth / 3.0f;
		unsigned int windowHeight = displayHeight / 2.0f + displayHeight / 6.0f;

		m_pLobbyWindow->SetWidth(windowWidth);
		m_pLobbyWindow->SetHeight(windowHeight);

		if (windowBG)
		{
			windowBG->SetWidth(windowWidth - diffX);
			windowBG->SetHeight(windowHeight - diffY);
		}

		UIElement* backButton = m_pLobbyWindow->GetChild("Back_Button", true);
		backButton->SetWidth(windowWidth);
		SubscribeToEvent(backButton, E_RELEASED,
		                 URHO3D_HANDLER(LobbyUI, BackButtonReleaseDelegate));

		ui->GetRoot()->AddChild(m_pLobbyWindow);

		m_pLobbyWindow->SetVisible(false);

		m_pNameEdit = (LineEdit*)m_pLobbyWindow->GetChild("Username_Line_Edit", true);
		Text* m_pLineEditText = m_pNameEdit->GetTextElement();
		m_pLineEditText->SetAlignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER);
		m_pLineEditText->SetFontSize(30);
		m_pNameEdit->SetCursorPosition(m_pLineEditText->GetWidth() / 2.0);
		m_pNameEdit->SetMaxLength(8);

		m_pWinsText = (Text*)m_pLobbyWindow->GetChild("Wins_Count", true);
		m_pLosesText = (Text*)m_pLobbyWindow->GetChild("Loses_Count", true);
		m_pTotalText = (Text*)m_pLobbyWindow->GetChild("Total_Count", true);

		CornersLogic* pCornersLogic = static_cast<CornersLogic*>(g_pApp->GetGameLogic());
		const PlayerData& playerData = pCornersLogic->GetFirstPlayer();

		if(playerData.m_Username != "Invalid")
		{
			m_pLineEditText->SetText(playerData.m_Username);
			m_pWinsText->SetText(String(playerData.m_Wins));
			m_pLosesText->SetText(String(playerData.m_Loses));
			m_pTotalText->SetText(String(playerData.m_Wins + playerData.m_Loses));
		}

		SubscribeToEvent(EVENT_DATA_CLIENT_FINISH_GAME, URHO3D_HANDLER(LobbyUI, ClientFinishGameDelegate));
	}
}

void LobbyUI::BackButtonReleaseDelegate(StringHash eventType, VariantMap& eventData)
{
	if(m_pLobbyWindow)
	{
		VSetVisible(false);
		SharedPtr<IScreenElement> mainWindow = m_pMainMenuView->GetMainUI();
		mainWindow->VSetVisible(true);
	}

	String name = m_pNameEdit->GetText();
	if(!name.Empty())
	{
		VariantMap data;
		data[SAVE_PLAYER_DATA::P_USERNAME] = name;
		SendEvent(EVENT_DATA_PLAYER_SAVE_DATA, data);
	}
}

void LobbyUI::ClientFinishGameDelegate(StringHash eventType, VariantMap& eventData)
{
	CornersLogic* pCornersLogic = static_cast<CornersLogic*>(g_pApp->GetGameLogic());
	const PlayerData& firstPlayer = pCornersLogic->GetFirstPlayer();

	m_pWinsText->SetText(String(firstPlayer.m_Wins));
	m_pLosesText->SetText(String(firstPlayer.m_Loses));
	m_pTotalText->SetText(String(firstPlayer.m_Wins + firstPlayer.m_Loses));
}


