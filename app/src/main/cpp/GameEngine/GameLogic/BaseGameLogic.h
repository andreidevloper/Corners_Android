#pragma once

class LevelManager;

enum BaseGameState : int
{
	BGS_Invalid,
	BGS_Initializing,
	BGS_MainMenu,
	BGS_WaitingForPlayers,
	BGS_LoadingGameEnvironment,
	BGS_WaitingForPlayersToLoadEnvironment,
	BGS_SpawningPlayersActors,
	BGS_Running
};

class BaseGameLogic : public IGameLogic
{
	// This is neccessary, in order that get access to GameViews.
	friend class GameEngineApp;

	URHO3D_OBJECT(BaseGameLogic, IGameLogic)
public:
	// Constructors
	BaseGameLogic(Context *context);
	virtual ~BaseGameLogic();

	// Init, run, shutdown
	virtual bool VInitialize();
	virtual void VOnUpdate(float timeStep);
	virtual void VShutdown();

	// Handle game state
	virtual void VChangeState(enum BaseGameState newState);
	virtual bool VLoadGame(String levelResource);

	// Manage game views
	virtual void VAddView(SharedPtr<IGameView> pView, NodeId nodeId = INVALID_NODE_ID);
	virtual void VRemoveView(SharedPtr<IGameView> pView);

	// Getter/Setters
	inline GameViewList& GetGameViews() { return m_GameViews; }
	inline Scene* GetScene() { return m_pScene; }
	inline BaseGameState GetCurrentGameState() { return m_State; }

protected:
	// Override this function to do any game-specific loading.
	virtual bool VLoadGameDelegate(String pLevelData) { return true; }

private:
	// Register factory and URHO3D_ATTRIBUTEs for the component so it can be created via CreateComponent, and loaded / saved
	virtual void VInitializeComponents() { };		
	
protected:
	virtual void InitializeAllDelegates();		// Register all delegates
	void DestroyAllDelegates();					// Unsubscribe from all events

	// Delegates
	void RequestDestroyActorDelegate(StringHash eventType, VariantMap& eventData);
	void RequestStartGameDelegate(StringHash eventType, VariantMap& eventData);
	void EnvironmentLoadedDelegate(StringHash eventType, VariantMap& eventData);

protected:
	float m_Lifetime;									// Indicates how long this game has been in session
	
	SharedPtr<Scene> m_pScene;									// Derived and Base Game logic control scene logic (HumanView get it)
	BaseGameState m_State;								// Current game state: loading, running, etc.
	BaseGameState m_NeededState;						// Which state set for next frame ? 
	GameViewList m_GameViews;							// Views that are attached to our game

	bool m_bIsRenderDiagnostic;							// Are we rendering diagnostics?

	int m_HumanGamesLoaded;
	int m_ExpectedPlayers;								// How many players will play with us ?
	int m_HumanPlayersAttached;							// How many players actually play with us
};

