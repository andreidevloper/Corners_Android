#include "CornersStd.h"

#include <GameEngine/GameLogic/BaseGameLogic.h>
#include <GameEngine/Audio/AudioSubsystem.h>

#include <GameEngine/Events/Client/ClientEvent.h>

#include "UserInterface/ScreenElements/Game/GameUI.h"
#include "UserInterface/ScreenElements/Game/CurrentPlayerUI.h"
#include "UserInterface/ScreenElements/Game/RetryUI.h"
#include "UserInterface/ScreenElements/Game/HistoryUI.h"

#include "Controllers/CheckerController.h"
#include "CornersHumanView.h"

CornersHumanView::CornersHumanView(Context* context, Renderer* renderer) : HumanView(context, renderer)
{
    m_pGameUI = SharedPtr<IScreenElement>(new GameUI(context, this));
    m_pCurrentPlayerUI = SharedPtr<IScreenElement>(new CurrentPlayerUI(context, this));
    m_pHistoryUI = SharedPtr<IScreenElement>(new HistoryUI(context, this));
    m_pRetryUI = SharedPtr<IScreenElement>(new RetryUI(context, this));

    VPushElement(m_pGameUI);
    VPushElement(m_pCurrentPlayerUI);
    VPushElement(m_pHistoryUI);
    VPushElement(m_pRetryUI);

    // Restore UI if needed
    m_pGameUI->VOnRestore();
    m_pCurrentPlayerUI->VOnRestore();
    m_pHistoryUI->VOnRestore();
    m_pRetryUI->VOnRestore();

    m_pGameUI->VSetVisible(true);
    m_pCurrentPlayerUI->VSetVisible(true);

    SubscribeToEvent(EVENT_DATA_SHOW_GAME_UI, URHO3D_HANDLER(CornersHumanView, ShowGameUIDelegate));
    SubscribeToEvent(EVENT_DATA_CLIENT_FINISH_TURN, URHO3D_HANDLER(CornersHumanView, ClientFinishTurnDelegate));
}

void CornersHumanView::VShutdown()
{
	HumanView::VShutdown();
	g_pAudio->UnloadGameSounds("Game");
}

bool CornersHumanView::VOnRestore()
{

	return true;
}


bool CornersHumanView::VLoadGameDelegate(SharedPtr<File> level)
{
    HumanView::VLoadGameDelegate(level);

    if (g_pAudio->LoadGameSoundsXML("Sound/Game.xml", true))
    {
        m_pClickBuffer = g_pAudio->VInitAudioBuffer(m_pScene, "Click");
    }

    // If Camera Node is not Null then there must have been a default camera
    if (m_pControlledNode == NULL)
    {
        // Create a scene node for the camera, which we will move around
        // The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
        m_pControlledNode = m_pScene->CreateChild(String("Camera").ToHash(), CreateMode::LOCAL);
        m_pControlledNode->CreateComponent<Camera>();

        // Set Position
        m_pControlledNode->SetPosition(Vector3(-400.0f, 60.0f, 200.0f));

        // Test Look at
        m_pControlledNode->LookAt(Vector3(0.0f, 0.0f, 0.0f));
        m_pControlledNode->GetComponent<Camera>()->SetFarClip(2000.0);

        // Set new Viewport using Camera
        m_pViewport = SharedPtr<Viewport>(new Viewport(context_, m_pScene, m_pControlledNode->GetComponent<Camera>()));
    }

    // Set Viewport
    m_pRenderer->SetViewport(0, m_pViewport);

    // Get Corners Game Logic
    if(m_pPointerHandler.Null())
    {
        CornersLogic* cornersLogic = (CornersLogic*) g_pApp->GetGameLogic();

        SharedPtr<CheckerController> checkerController = SharedPtr<CheckerController>(
                new CheckerController(context_, cornersLogic, FIRST_PLAYER, m_pScene,
                                      m_pControlledNode->GetComponent<Camera>()));
        m_pPointerHandler = checkerController;

        Vector3 pos = m_pControlledNode->GetPosition();
        pos.z_ = 9.5f;
        m_pControlledNode->SetPosition(pos);
    }
}


void CornersHumanView::ClientFinishTurnDelegate(StringHash eventType, VariantMap& eventData)
{
	// Only if set autoPlay = false for background
	m_pClickBuffer->VPlay(0.5f, false);
}

void CornersHumanView::ShowGameUIDelegate(StringHash eventType, VariantMap &eventData)
{
    m_pGameUI->VSetVisible(true);
    m_pCurrentPlayerUI->VSetVisible(true);
}
