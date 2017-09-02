#include "GameEngineStd.h"

#include "UserInterface/HumanView/HumanView.h"

#include "Events/GameEngineEvents.h"

#include "BaseGameLogic.h"

BaseGameLogic::BaseGameLogic(Context *context) : IGameLogic(context)
{
	m_bIsRenderDiagnostic = false;

	m_HumanPlayersAttached = 0;
	m_ExpectedPlayers = 0;
	m_HumanGamesLoaded = 0;

	m_pScene = new Scene(context_);

	m_State = BGS_Invalid;
	m_NeededState = BGS_Invalid;
}

BaseGameLogic::~BaseGameLogic()
{
	
}

// ----------------------------------------------------------
// Init, run, shutdown
// ----------------------------------------------------------
bool BaseGameLogic::VInitialize()
{
	m_State = BGS_Initializing;

	// Change random seed
	SetRandomSeed(GetRandomSeed());

	VInitializeComponents();
	InitializeAllDelegates();
	return true;
}

void BaseGameLogic::VOnUpdate(float timeStep)
{
	m_Lifetime += timeStep;

	switch (m_State)
	{
		case BGS_Initializing:
		{
			// If we get to here we're ready to attach players
			VChangeState(BGS_MainMenu);
			break;
		}

		case BGS_MainMenu:
		{
			
			break;
		}


		case BGS_WaitingForPlayers:
		{
			if (m_ExpectedPlayers == m_HumanPlayersAttached)
			{
				// The server sends us the level name as a part of the login message. 
				// We have to wait until it arrives before loading the level
				if (!g_pApp->GetGameOptions()->GetSceneName().Empty())
				{
					VChangeState(BGS_LoadingGameEnvironment);
				}
			}
			break;
		}

		case BGS_WaitingForPlayersToLoadEnvironment:
		{
			if (m_ExpectedPlayers <= m_HumanGamesLoaded)
			{
				VChangeState(BGS_SpawningPlayersActors);
			}

			break;
		}

		case BGS_SpawningPlayersActors:
		{
			VChangeState(BGS_Running);
			break;
		}
			
		case BGS_Running:
		{
			break;
		}
	}

	// update all game views
	for (GameViewList::Iterator it = m_GameViews.Begin(); it != m_GameViews.End(); ++it)
	{
		(*it)->VOnUpdate(timeStep);
	}
}

void BaseGameLogic::VShutdown()
{
	// Call virtual shut down procedure.
	for (GameViewList::Iterator it = m_GameViews.Begin(); it != m_GameViews.End(); it++)
	{
		(*it)->VShutdown();
	}

	// Delete each game view from list.
	while (!m_GameViews.Empty())
	{
		m_GameViews.PopFront();
	}

	DestroyAllDelegates();

	m_State = BGS_Invalid;
}

// ----------------------------------------------------------
// Handle game state
// ----------------------------------------------------------

void BaseGameLogic::VChangeState(enum BaseGameState newState)
{

	if (newState == BGS_WaitingForPlayers)
	{
		// Note: Split screen support would require this to change!
		m_ExpectedPlayers = 1;
	}
	else if (newState == BGS_LoadingGameEnvironment)
	{
		m_State = newState;
		if (!g_pApp->VLoadGame())
		{
			URHO3D_LOGERROR("The game failed to load. Abort game.");
			g_pApp->AbortGame();
		}
		else
		{
			VChangeState(BGS_WaitingForPlayersToLoadEnvironment);			// we must wait for all human player to report their environments are loaded.
			return;
		}
	}

	m_State = newState;
}


bool BaseGameLogic::VLoadGame(String levelResource)
{
	SharedPtr<File> file = g_pApp->GetResCache()->GetFile(levelResource);
	if (file.NotNull())
	{
		m_pScene->LoadXML(*file);
	}
	else
	{
		URHO3D_LOGERROR("Failed to find scene file in this path: " + levelResource);
		return false;
	}

	// initialize all human views
	for (auto it = m_GameViews.Begin(); it != m_GameViews.End(); ++it)
	{
		SharedPtr<IGameView> pView = *it;
		if (pView->VGetType() == GameView_Human)
		{
			SharedPtr<HumanView> pHumanView = StaticCast<HumanView, IGameView>(pView);
			pHumanView->LoadGame(file);
		}
	}

	// call the delegate load function
	if (!VLoadGameDelegate(levelResource))
		return false;  // no error message here because it's assumed VLoadGameDelegate() kicked out the error

	SendEvent(EVENT_DATA_ENVIRONMENT_LOADED);
		
	return true;
}

// ----------------------------------------------------------
// Manage game views
// ----------------------------------------------------------

void BaseGameLogic::VAddView(SharedPtr<IGameView> pView, NodeId nodeId)
{
	// This makes sure that all views have a non-zero view id.
	int viewId = static_cast<int>(m_GameViews.Size());
	m_GameViews.Push(pView);
	pView->VOnAttach(viewId, nodeId);
	if (!pView->VOnRestore())
	{
		URHO3D_LOGERROR("Failed to initialize game view");
	}
}

void BaseGameLogic::VRemoveView(SharedPtr<IGameView> pView)
{
	GameViewList::Iterator it = m_GameViews.Find(pView);
	if (it->NotNull())
	{
		m_GameViews.Erase(it);
	}
}


// ----------------------------------------------------------
// Delegates routines
// ----------------------------------------------------------

void BaseGameLogic::InitializeAllDelegates()
{
	SubscribeToEvent(EVENT_DATA_START_GAME, URHO3D_HANDLER(BaseGameLogic, RequestStartGameDelegate));
	SubscribeToEvent(EVENT_DATA_ENVIRONMENT_LOADED, URHO3D_HANDLER(BaseGameLogic, EnvironmentLoadedDelegate));
}

void BaseGameLogic::DestroyAllDelegates()
{
	UnsubscribeFromAllEvents();
}

void BaseGameLogic::RequestStartGameDelegate(StringHash eventType, VariantMap& eventData)
{
	VChangeState(BGS_WaitingForPlayers);
}

void BaseGameLogic::EnvironmentLoadedDelegate(StringHash eventType, VariantMap& eventData)
{
	m_HumanGamesLoaded++;
}