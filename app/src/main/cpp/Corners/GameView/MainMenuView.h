#pragma once

#include <GameEngine/UserInterface/HumanView/HumanView.h>

class MainMenuView : public HumanView
{
URHO3D_OBJECT(MainMenuView, HumanView)

public:
	 MainMenuView(Context* context, Renderer* renderer);

     SharedPtr<IScreenElement> GetMainUI() { return m_pMainUI; }
	 SharedPtr<IScreenElement> GetLobbyUI() { return m_pLobbyUI; }
	 SharedPtr<IScreenElement> GetOptionsUI() { return m_pOptionsUI; }
	 SharedPtr<IScreenElement> GetCreditsUI() { return m_pCreditsUI; }

protected:
	void ShowMainMenuDelegate(StringHash eventType, VariantMap& eventData);

public:
	 // All mainmenu screen elements
	 SharedPtr<IScreenElement> m_pMainUI;
	 SharedPtr<IScreenElement> m_pLobbyUI;
	 SharedPtr<IScreenElement> m_pOptionsUI;
	 SharedPtr<IScreenElement> m_pCreditsUI;
};
