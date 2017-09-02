#pragma once

#include <GameEngine/UserInterface/HumanView/HumanView.h>

class CornersHumanView : public HumanView
{
	URHO3D_OBJECT(CornersHumanView, HumanView)

public:
	CornersHumanView(Context* context, Renderer* renderer);

	// Initialize humam view
	virtual bool VOnRestore();
	// Shutdown view.
	virtual void VShutdown();


    SharedPtr<IScreenElement> GetGameUI()  { return m_pGameUI; }
    SharedPtr<IScreenElement> GetHistoryUI()  { return m_pHistoryUI; }
    SharedPtr<IScreenElement> GetCurrentPlayerUI() { return m_pCurrentPlayerUI; }

protected:
    // Loading game environment.
    virtual bool VLoadGameDelegate(SharedPtr<File> level);

    void ClientFinishTurnDelegate(StringHash eventType, VariantMap& eventData);
	void ShowGameUIDelegate(StringHash eventType, VariantMap& eventData);

protected:
    SharedPtr<IScreenElement> m_pGameUI;
    SharedPtr<IScreenElement> m_pHistoryUI;
    SharedPtr<IScreenElement> m_pCurrentPlayerUI;
	SharedPtr<IScreenElement> m_pRetryUI;


	IAudioBuffer* m_pClickBuffer;
};

