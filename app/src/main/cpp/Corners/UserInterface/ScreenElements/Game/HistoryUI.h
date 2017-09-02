#pragma once

#include "GameEngine/UserInterface/UserInterface.h"

class CornersHumanView;

class HistoryUI : public BaseUI
{
    URHO3D_OBJECT(HistoryUI, BaseUI);

public:
    HistoryUI(Context* context, CornersHumanView * cornersHumanView);

    virtual bool VOnRestore();
    virtual void VOnShutdown();
    virtual void VSetVisible(bool visible);

    // Helper functions
    void CreateHistoryWindow();

    void BackButtonReleaseDelegate(StringHash eventType, VariantMap& eventData);
    void ClientFinishTurnDelegate(StringHash eventType, VariantMap& eventData);
    void TryPlayAgainGameDelegate(StringHash eventType, VariantMap& eventData);

private:
    Text* CreateText();

protected:
    CornersHumanView* m_pCornersHumanView;
    SharedPtr<Window> m_pHistoryWindow;

    ListView* m_pFirstPlayerHistory;
    ListView* m_pSecondPlayerHistory;

    unsigned int m_StepCount;
};
