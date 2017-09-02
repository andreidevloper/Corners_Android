#include "CornersStd.h"

#include <GameEngine/Events/Client/ClientEvent.h>

#include "GameView/CornersHumanView.h"
#include "GameLogic/CornersLogic.h"
#include "HistoryUI.h"

HistoryUI::HistoryUI(Context* context, CornersHumanView* cornersHumanView) : BaseUI(context),
                                                                             m_pCornersHumanView(cornersHumanView),
                                                                             m_pHistoryWindow(nullptr)
{
    m_StepCount = 1;
    SubscribeToEvent(EVENT_DATA_CLIENT_TRY_PLAY_AGAIN_GAME, URHO3D_HANDLER(HistoryUI, TryPlayAgainGameDelegate));
}

void HistoryUI::VOnShutdown()
{
	VSetVisible(false);
}

bool HistoryUI::VOnRestore()
{
    if (!m_bIsInitialized)
    {
        CreateHistoryWindow();
        m_bIsInitialized = true;
    }

    return true;
}

void HistoryUI::VSetVisible(bool visible)
{
    m_bIsVisible = visible;

    if(m_pHistoryWindow.NotNull())
    {
	    m_pHistoryWindow->SetVisible(visible);
    }
}

void HistoryUI::CreateHistoryWindow()
{
    if(!m_pHistoryWindow)
    {
		// Generate Layout
		String Layout = String(ScreenPath[LayoutUtility::GetLayout()]);

		// Generate Path
		String xmlLayout = String("UI/Layout/")+Layout+String("/Turns_History.xml");

		UI* pUI = GetSubsystem<UI>();
		XMLFile* file = g_pApp->GetResCache()->GetResource<XMLFile>(xmlLayout);

        m_pHistoryWindow = StaticCast<Window>(pUI->LoadLayout(file));
        UIElement* windowBG = m_pHistoryWindow->GetChild("Credits_BG", true);

        unsigned int diffX = m_pHistoryWindow->GetWidth() - windowBG->GetWidth();
        unsigned int diffY = m_pHistoryWindow->GetHeight() - windowBG->GetHeight();

        unsigned int displayWidth = g_pApp->GetGraphics()->GetWidth();
        unsigned int displayHeight = g_pApp->GetGraphics()->GetHeight();

        unsigned int windowWidth = displayWidth / 2.0f + displayWidth / 3.0f;
        unsigned int windowHeight = displayHeight / 2.0f + displayHeight / 6.0f;

        m_pHistoryWindow->SetWidth(windowWidth);
        m_pHistoryWindow->SetHeight(windowHeight);

        m_pHistoryWindow->SetVisible(false);

        if(windowBG)
        {
            windowBG->SetWidth(windowWidth - diffX);
            windowBG->SetHeight(windowHeight - diffY);
        }

        UIElement* backButton = m_pHistoryWindow->GetChild("Back_Button", true);

        m_pFirstPlayerHistory = static_cast<ListView*>(m_pHistoryWindow->GetChild("1_Player_History_List_View", true));
        int height =  windowHeight - diffY - 225.0f - backButton->GetHeight();
        m_pFirstPlayerHistory->SetHeight(height);

        ScrollBar* firstPlayerScrollbar = static_cast<ScrollBar*>(m_pHistoryWindow->GetChild("1_Player_History_Scroll_Bar", true));
        firstPlayerScrollbar->SetHeight(height);

        m_pSecondPlayerHistory = static_cast<ListView*>(m_pHistoryWindow->GetChild("2_Player_History_List_View", true));
        m_pSecondPlayerHistory->SetHeight(height);

        ScrollBar* secondPlayerScrollbar = static_cast<ScrollBar*>(m_pHistoryWindow->GetChild("2_Player_History_Scroll_Bar", true));
        secondPlayerScrollbar->SetHeight(height);

        backButton->SetWidth(windowWidth);
        SubscribeToEvent(backButton, E_RELEASED, URHO3D_HANDLER(HistoryUI, BackButtonReleaseDelegate));

        pUI->GetRoot()->AddChild(m_pHistoryWindow);

        SubscribeToEvent(EVENT_DATA_CLIENT_FINISH_TURN, URHO3D_HANDLER(HistoryUI, ClientFinishTurnDelegate));
    }
}

void HistoryUI::BackButtonReleaseDelegate(StringHash eventType, VariantMap& eventData)
{
	if(m_pHistoryWindow)
	{
		VSetVisible(false);
		SharedPtr<IScreenElement> gameWindow = m_pCornersHumanView->GetGameUI();
		gameWindow->VSetVisible(true);
	}
}

void HistoryUI::ClientFinishTurnDelegate(StringHash eventType, VariantMap& eventData)
{


	PlayerType currentPlayer = (PlayerType)eventData[CLIENT_FINISH_TURN::P_PLAYER_TYPE].GetInt();
	Vector2 oldCheckerPos = eventData[CLIENT_FINISH_TURN::P_OLD_POSITION].GetVector2();
	Vector2 newCheckerPos = eventData[CLIENT_FINISH_TURN::P_NEW_POSITION].GetVector2();

	Text* pNewText = CreateText();
	String oldPos = oldCheckerPos.ToString();
	oldPos.Replace(' ', ';', true);

	String newPos = newCheckerPos.ToString();
	newPos.Replace(' ', ';', true);

	pNewText->SetText(String(m_StepCount) + ". [" + oldPos + "] -> " + "[" + newPos + "]");

	if(currentPlayer == SECOND_PLAYER)
	{
		m_pFirstPlayerHistory->AddItem(pNewText);
	}
	else if(currentPlayer == FIRST_PLAYER)
	{
		m_pSecondPlayerHistory->AddItem(pNewText);
	}
    m_StepCount++;
}

void HistoryUI::TryPlayAgainGameDelegate(StringHash eventType, VariantMap& eventData)
{
    m_StepCount = 1;
    m_pFirstPlayerHistory->RemoveAllItems();
    m_pSecondPlayerHistory->RemoveAllItems();
}

Text* HistoryUI::CreateText()
{
	XMLFile* style = g_pApp->GetResCache()->GetResource<XMLFile>("UI/DefaultStyle.xml");
	Text* pText = new Text(context_);
	pText->SetDefaultStyle(style);
	pText->SetStyleAuto();
	pText->SetFont(g_pApp->GetResCache()->GetResource<Font>("UI/Fonts/bluehighway.ttf"), 25);
	pText->SetColor(Color(1.0f, 1.0f, 1.0f));
	//pText->SetAlignment(HA_CENTER, VA_CENTER);
	return pText;
}