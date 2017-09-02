#pragma once

//----------------------------------------------------------------------------------------------------
// CornersApp - Application class contains all necessary components in order to run program correctly.
//----------------------------------------------------------------------------------------------------

class CornersDatabase;

class CornersApp : public GameEngineApp
{
public:
	// Constructors
	CornersApp(Context* context);
	virtual ~CornersApp();

	// Application class overrided functions
	virtual void Setup();						// Called before engine initialization. Logging system and
	// user specific game options init here

	virtual void Start();						// Called after engine initialization. Setup application & subscribe to events here
	virtual void Stop();						// Perform optional cleanup after main loop has terminated

	// Create game logic and game views
	bool virtual VCreateViewLogic();			// Create specific game logic and game view.

	virtual String GetWindowIcon() { return String::EMPTY; }
	virtual String GetWindowTitle() { return "Corners"; }

    inline CornersGameOptions* GetCornersGameOptions() { return (CornersGameOptions*)m_pBaseGameOptions; }
    inline CornersDatabase* GetCornersDatabase() { return (CornersDatabase*)m_pDatabase; }

protected:
    virtual bool VCreateGameOptions();
    virtual bool VCreateDatabase();
};
