#include "CornersStd.h"

#include <GameEngine/Events/Client/ClientEvent.h>
#include <GameEngine/Events/GameEngineEvents.h>
#include <GameEngine/GameLogic/BaseGameLogic.h>

#include <GameEngine/UserInterface/LayoutUtility.h>
#include <UserInterface/ScreenElements/Game/GameUI.h>

#include "GameView/MainMenuView.h"
#include "MainMenuUI.h"

MainMenuUI::MainMenuUI(Context* context, MainMenuView* menu) : BaseUI(context),
                                                                   m_pMainWindow(nullptr),
                                                                   m_pMainMenuView(menu),
                                                                   m_bIsGameStarted(false)
{
    SubscribeToEvent(EVENT_DATA_CLIENT_FINISH_GAME, URHO3D_HANDLER(MainMenuUI, ClientFinishGameDelegate));
    SubscribeToEvent(EVENT_DATA_CLIENT_TRY_PLAY_AGAIN_GAME, URHO3D_HANDLER(MainMenuUI, ClientTryPlayAgainGameDelegate));
}

MainMenuUI::~MainMenuUI()
{

}

void MainMenuUI::VOnShutdown()
{
	VSetVisible(false);
}


bool MainMenuUI::VOnRestore()
{
	if (!m_bIsInitialized)
	{
		CreateMainWindow();
        m_bIsInitialized = true;
	}

	return true;
}

void MainMenuUI::VSetVisible(bool visible)
{
	m_bIsVisible = visible;

	if (m_pMainWindow)
	{
		m_pMainWindow->SetVisible(visible);
        m_pMainWindow->SetPriority(2);
        URHO3D_LOGINFO(String(m_pMainWindow->GetPriority()));
        URHO3D_LOGINFO(String(m_pBackground->GetPriority()));
        URHO3D_LOGINFO(String(m_pPattern->GetPriority()));
    }
}


bool MainMenuUI::VOnMsgProc(AppMsg message)
{

	return false;
}


void MainMenuUI::CreateMainWindow()
{
	if (!m_pMainWindow)
	{
		// Generate Layout
		String Layout = String(ScreenPath[LayoutUtility::GetLayout()]);

		// Generate Path
		String xmlLayout = String("UI/Layout/")+Layout+String("/MainMenu.xml");

		UI* ui = GetSubsystem<UI>();
		XMLFile* file = g_pApp->GetResCache()->GetResource<XMLFile>(xmlLayout);
		m_pMainWindow = StaticCast<Window>(ui->LoadLayout(file));

		unsigned int WindowWidth = g_pApp->GetGraphics()->GetWidth();
		unsigned int WindowHeight = g_pApp->GetGraphics()->GetHeight();
		m_pMainWindow->SetOpacity(0.95f);
		m_pMainWindow->SetWidth(WindowWidth / 2.0f + WindowWidth / 3.0f);
		m_pMainWindow->SetHeight(WindowHeight / 2.0f + WindowHeight / 5.0f);

		//  UIElement* windowBG = element->GetChild(String("Credits_BG"), true);
		//  windowBG->SetWidth(WindowWidth / 2.0f + WindowWidth / 3.0f - 2.0f);
		//  windowBG->SetHeight(WindowHeight / 2.0f + WindowHeight / 3.0f - 2.0f);

		unsigned int menuWidthPerElement = (WindowWidth / 2.0f + WindowWidth / 3.0f);
		unsigned int menuHeightPerElement = (WindowHeight / 2.0f + WindowHeight / 5.0f) / 5.0f;
		unsigned int positionY = 0;

		int fontSize;
		if (menuHeightPerElement >= 200)
		{
			fontSize = 90;
		}
		else
		{
			fontSize = 60;
		}

		UIElement* startButton = m_pMainWindow->GetChild(String("Start"), true);
		SubscribeToEvent(startButton, E_RELEASED,
		                 URHO3D_HANDLER(MainMenuUI, StartButtonReleaseDelegate));

		startButton->SetOpacity(1.0f);
		startButton->SetWidth(menuWidthPerElement);
		startButton->SetHeight(menuHeightPerElement);

		Text* startText = static_cast<Text*>(m_pMainWindow->GetChild(String("Start_Text"), true));
		startText->SetFontSize(fontSize);

		UIElement* lobbyButton = m_pMainWindow->GetChild(String("Lobby"), true);
		SubscribeToEvent(lobbyButton, E_RELEASED,
		                 URHO3D_HANDLER(MainMenuUI, LobbyButtonReleaseDelegate));

		lobbyButton->SetWidth(menuWidthPerElement);
		lobbyButton->SetHeight(menuHeightPerElement);

		positionY += menuHeightPerElement;
		IntVector2 currentPos = lobbyButton->GetPosition();
		lobbyButton->SetPosition(currentPos.x_, positionY);

		Text* lobbyText = static_cast<Text*>(m_pMainWindow->GetChild(String("Lobby_Text"), true));
		lobbyText->SetFontSize(fontSize);

		UIElement* optionsButton = m_pMainWindow->GetChild(String("Options"), true);
		SubscribeToEvent(optionsButton, E_RELEASED,
		                 URHO3D_HANDLER(MainMenuUI, OptionsButtonReleaseDelegate));

		optionsButton->SetWidth(menuWidthPerElement);
		optionsButton->SetHeight(menuHeightPerElement);

		positionY += menuHeightPerElement;
		currentPos = optionsButton->GetPosition();
		optionsButton->SetPosition(currentPos.x_, positionY);

		Text* optionsText = static_cast<Text*>(m_pMainWindow->GetChild(String("Options_Text"), true));
		optionsText->SetFontSize(fontSize);

		UIElement* creditsButton = m_pMainWindow->GetChild(String("Credits"), true);
		SubscribeToEvent(creditsButton, E_RELEASED,
		                 URHO3D_HANDLER(MainMenuUI, CreditsButtonReleaseDelegate));

		creditsButton->SetWidth(menuWidthPerElement);
		creditsButton->SetHeight(menuHeightPerElement);

		positionY += menuHeightPerElement;
		currentPos = creditsButton->GetPosition();
		creditsButton->SetPosition(currentPos.x_, positionY);

		Text* creditsText = static_cast<Text*>(m_pMainWindow->GetChild(String("Credits_Text"), true));
		creditsText->SetFontSize(fontSize);

		UIElement* quitButton = m_pMainWindow->GetChild(String("Quit"), true);
		SubscribeToEvent(quitButton, E_RELEASED,
		                 URHO3D_HANDLER(MainMenuUI, QuitButtonReleaseDelegate));

		quitButton->SetWidth(menuWidthPerElement);
		quitButton->SetHeight(menuHeightPerElement);

		positionY += menuHeightPerElement;
		currentPos = quitButton->GetPosition();
		quitButton->SetPosition(currentPos.x_, positionY);

		Text* quitText = static_cast<Text*>(m_pMainWindow->GetChild(String("Quit_Text"), true));
		quitText->SetFontSize(fontSize);

		m_pMainWindow->SetVisible(false);

        m_pBackground = ui->LoadLayout(g_pApp->GetResCache()->GetResource<XMLFile>(String("UI/Layout/")+Layout+String("/Background.xml")));
        m_pPattern = ui->LoadLayout(g_pApp->GetResCache()->GetResource<XMLFile>(String("UI/Layout/")+Layout+String("/Pattern.xml")));

        GetSubsystem<UI>()->GetRoot()->AddChild(m_pMainWindow);
        GetSubsystem<UI>()->GetRoot()->AddChild(m_pBackground);
        GetSubsystem<UI>()->GetRoot()->AddChild(m_pPattern);
	}
}

void MainMenuUI::StartButtonReleaseDelegate(StringHash eventType, VariantMap& eventData)
{
	if (m_pMainMenuView)
	{
		VSetVisible(false);
        m_pPattern->SetVisible(m_bIsVisible);
        m_pBackground->SetVisible(m_bIsVisible);

	    if(!m_bIsGameStarted)
	    {
		   SetStartButtonText("CONTINUE");

		   if(g_pApp->GetGameLogic()->GetCurrentGameState() == BGS_Running)
		   {
               // If BGS_Running it means that we have played game before
               // Reset checkers positions
			   SendEvent(EVENT_DATA_CLIENT_TRY_PLAY_AGAIN_GAME);
               // Start button is pressed after finished playing, show game UI
               SendEvent(EVENT_DATA_SHOW_GAME_UI);
		   }
		   else
		   {
               // If not, then it is first game
			   // Send event to start game (spawn draughts GA, create controller, enable game logic
			   SendEvent(EVENT_DATA_START_GAME);
		   }

		   m_bIsGameStarted = true;
	   }
       else
       {
           // Continue button is pressed after finished playing, show game UI
           SendEvent(EVENT_DATA_SHOW_GAME_UI);
       }
	}
	else
	{
		URHO3D_LOGERROR("Failed to find mainmenu view !");
	}
}

void MainMenuUI::LobbyButtonReleaseDelegate(StringHash eventType, VariantMap& eventData)
{
	if (m_pMainMenuView)
	{
		SharedPtr<IScreenElement> lobbyWindow = m_pMainMenuView->GetLobbyUI();
		VSetVisible(false);
		lobbyWindow->VSetVisible(true);
	}
	else
	{
		URHO3D_LOGINFO("Failed to find mainmenu view !");
	}
}

void MainMenuUI::OptionsButtonReleaseDelegate(StringHash eventType, VariantMap& eventData)
{
	if (m_pMainMenuView)
	{
		SharedPtr<IScreenElement> optionsWindow = m_pMainMenuView->GetOptionsUI();
		VSetVisible(false);
		optionsWindow->VSetVisible(true);
	}
	else
	{
        URHO3D_LOGINFO("Failed to find mainmenu view !");
	}
}

void MainMenuUI::CreditsButtonReleaseDelegate(StringHash eventType, VariantMap& eventData)
{
	if (m_pMainMenuView)
	{
		SharedPtr<IScreenElement> creditsWindow = m_pMainMenuView->GetCreditsUI();
		VSetVisible(false);
		creditsWindow->VSetVisible(true);
	}
	else
	{
        URHO3D_LOGINFO("Failed to find mainmenu view !");
	}
}

void MainMenuUI::QuitButtonReleaseDelegate(StringHash eventType, VariantMap& eventData)
{
	g_pApp->AbortGame();
}

void MainMenuUI::SetStartButtonText(String text)
{
	Text* startText = static_cast<Text*>(m_pMainWindow->GetChild("Start_Text", true));
	startText->SetText(text);
}

void MainMenuUI::ClientFinishGameDelegate(StringHash eventType, VariantMap& eventData)
{
	SetStartButtonText("START");
	m_bIsGameStarted = false;
}

void MainMenuUI::ClientTryPlayAgainGameDelegate(StringHash eventType, VariantMap& eventData)
{
	SetStartButtonText("CONTINUE");
	m_bIsGameStarted = true;
}
