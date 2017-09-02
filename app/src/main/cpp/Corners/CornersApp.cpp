#include "CornersStd.h"

#include "Database/CornersDatabase.h"
#include "GameLogic/CornersLogic.h"
#include "GameView/MainMenuView.h"

#include "CornersApp.h"

// Application entry-point
URHO3D_DEFINE_APPLICATION_MAIN(CornersApp)

CornersApp::CornersApp(Context* context) : GameEngineApp(context)
{
	g_pApp = this;
	m_bIsInit = false;
}

CornersApp::~CornersApp()
{

}


void CornersApp::Setup()
{
    GameEngineApp::Setup();
}

bool CornersApp::VCreateGameOptions()
{
    m_pBaseGameOptions = new CornersGameOptions();
}

bool CornersApp::VCreateDatabase()
{
    m_pDatabase = new CornersDatabase(context_);

    return true;
}

void CornersApp::Start()
{
    GameEngineApp::Start();
	m_bIsInit = true;
}

void CornersApp::Stop()
{
    GameEngineApp::Stop();

}

// Call GameEngine code
bool CornersApp::VCreateViewLogic()
{
	m_pGameLogic = new CornersLogic(context_);
	context_->RegisterSubsystem(m_pGameLogic);  // Urho3D free subsystem itself after memroy allocation

	if (!m_pGameLogic->VInitialize())
	{
		URHO3D_LOGERROR("Game logic init failed");
		m_bIsInit = false;
	}

    SharedPtr<IGameView> menuView = SharedPtr<IGameView>(new MainMenuView(context_, m_pRenderer));
    m_pGameLogic->VAddView(menuView);

    URHO3D_LOGDEBUG("Game logic successfully initialized");

	return true;
}

