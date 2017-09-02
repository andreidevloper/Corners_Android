#pragma once

//----------------------------------------------------------------------------------------------------
// GameEngineApp - Application class contains all necessary components in order to run app correctly.
//----------------------------------------------------------------------------------------------------

#include "Mainloop/Initialization.h"

using namespace Urho3D;

class BaseGameLogic;
class DatabaseSubsystem;

class GameEngineApp : public Application
{
public:
	GameEngineApp(Context* context);

	virtual void Setup();	// Called before engine initialization. Logging system and
							// user specific game options init here

	virtual void Start();	// Called after engine initialization. Setup application & subscribe to events here
	virtual void Stop();	// Perform optional cleanup after main loop has terminated
	
	void AbortGame();

	// Initialize instance. Set startup parameters for window and other subsystems
	void InitInstance(int screenWidth, int screenHeight, bool windowMode, bool vsync, int multisample, bool triplebuffer);	

	// Create game logic and game views
	bool virtual VCreateViewLogic() = 0;			// Create specific game logic and game view.

	// Game specific. Entry point to load the game.
	// It can be different loading steps for one game depend on platform. (pc, mobile, ...)
	bool VLoadGame();

	// Delegates
	void UpdateDelegate(StringHash eventType, VariantMap& eventData);		// Update delegeta to send update messsage
																			// to game logic
	// Message handling
	bool OnMessageProc(AppMsg msg);

	// Database
	DbConnection* ConnectToDB(String pathToDb);

	// Input/output delegates
	void KeyDownDelegate(StringHash eventType, VariantMap& eventData);		// Handle keyboarad's button down event
	void KeyUpDelegate(StringHash eventType, VariantMap& eventData);		// Handle keyboarad's button down event
	void MouseMoveDelegate(StringHash eventType, VariantMap& eventData);	// Handle mouse's event
	void MouseDownDelegate(StringHash eventType, VariantMap& eventData);	// Handle mouse's event
	void MouseUpDelegate(StringHash eventType, VariantMap& eventData);		// Handle mouse's event
    void TouchBeginDelegate(StringHash eventType, VariantMap& eventData);	// Handle screen touch event

	// Window delegates
	void InputFocusDelegate(StringHash eventType, VariantMap& eventData);	// Handle when focus is lost (e.x. window minimized)

	// Getter/Setters
	inline SharedPtr<Engine> GetEngine() { return engine_;  }
	inline FileSystem* GetFileSystem() { return m_pFileSystem; }
	inline Renderer* GetRenderer() { return m_pRenderer; }
	inline Graphics* GetGraphics() { return m_pGraphics; }
    inline Input* GetInput() { return m_pInput; }

	inline ResourceCache* GetResCache() { return m_pResourceCache; }
	inline BaseGameLogic* GetGameLogic() const { return m_pGameLogic; }

    inline DatabaseSubsystem* GetDatabase() const { return m_pDatabase; }

    // Game specific getters/setters
	virtual String GetWindowIcon() = 0;
	virtual String GetWindowTitle() = 0;

	inline String GetCurrentWorkDirectory() { return m_CurrentWorkDirectory; }
	inline String GetSaveDirectory() { return m_SaveDirectory; }
	inline IntVector2 GetCurrentWindowSize() { return m_pBaseGameOptions->GetWindowsOptions().m_ScreenSize; }
	inline BaseGameOptions* GetGameOptions() { return m_pBaseGameOptions;  }

	inline bool IsTouchEnabled() { return m_bIsTouchEnabled; }
    inline void SetTouchEnabled(bool touchEnabled) { m_bIsTouchEnabled = touchEnabled; }

protected:
	 bool CreateDebugHudAndConsole(XMLFile* style);

	 virtual bool VCreateGameOptions();
     virtual bool VCreateDatabase();

private:
	bool CreateCursor();

protected:
    BaseGameOptions* m_pBaseGameOptions;			// Basic game options

	Graphics* m_pGraphics;						// Application window handling and parameters.
	Renderer* m_pRenderer;						// 3D rendering capabilities, scene rendering.

	ResourceCache* m_pResourceCache;	        // Resource cache for constrant resources
	FileSystem* m_pFileSystem;					// File system access

	Input* m_pInput;							// Input device access (can choose poll or event-based mode)

	BaseGameLogic* m_pGameLogic;				// Base game logic (override in future simulation)

    DatabaseSubsystem* m_pDatabase;             // Base database

	String m_CurrentWorkDirectory;
	String m_SaveDirectory;

	// Flag to indicate whether touch input has been enabled
	bool m_bIsTouchEnabled;

	bool m_bIsInit;								// True - already initialized all subsystems. Simulation can be started.

private:
    IntVector2 m_LastMousePos;             		// For KeyDown event

	// Cursor
	Cursor* m_pCurrentCursor;						// Current cursor, that handle UI system.
	CursorShape m_CurrentCursorShape;			// Keep track of cursor state
	String m_CurrentCursorShapeString;
	bool m_bCustomCursorShape;

};

extern GameEngineApp* g_pApp;					// Global pointer, which is visible for whole program.

