#include "CornersStd.h"

#include "UserInterface/ScreenElements/MainMenu/MainMenuUI.h"
#include "UserInterface/ScreenElements/Credits/CreditsUI.h"
#include "UserInterface/ScreenElements/Lobby/LobbyUI.h"
#include "UserInterface/ScreenElements/Options/OptionsUI.h"

#include "MainMenuView.h"

MainMenuView::MainMenuView(Context* context, Renderer* renderer) : HumanView(context, renderer)
{
	m_pMainUI = SharedPtr<IScreenElement>(new MainMenuUI(context, this));
	m_pLobbyUI = SharedPtr<IScreenElement>(new LobbyUI(context, this));
	m_pOptionsUI = SharedPtr<IScreenElement>(new OptionsWindowUI(context, this));
	m_pCreditsUI = SharedPtr<IScreenElement>(new CreditsUI(context, this));

	VPushElement(m_pMainUI);
	VPushElement(m_pLobbyUI);
	VPushElement(m_pOptionsUI);
	VPushElement(m_pCreditsUI);

	// Restore UI if needed
	m_pMainUI->VOnRestore();
	m_pLobbyUI->VOnRestore();
	m_pOptionsUI->VOnRestore();
	m_pCreditsUI->VOnRestore();

    m_pMainUI->VSetVisible(true);

    SubscribeToEvent(EVENT_DATA_SHOW_MAIN_MENU_UI, URHO3D_HANDLER(MainMenuView, ShowMainMenuDelegate));
}

void MainMenuView::ShowMainMenuDelegate(StringHash eventType, VariantMap& eventData)
{
    m_pMainUI->VSetVisible(true);
}