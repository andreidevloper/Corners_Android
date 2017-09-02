#include "GameEngineStd.h"
#include "Database/DatabaseSubsystem.h"
#include "GameLogic/BaseGameLogic.h"

#include "Audio/AudioSubsystem.h"

GameEngineApp* g_pApp = nullptr;

GameEngineApp::GameEngineApp(Context* context) : Application(context)
{
	g_pApp = this;
	m_bIsInit = false;

    m_bIsTouchEnabled = false;
    m_pDatabase = NULL;
}

void GameEngineApp::Setup()
{
	m_bIsInit = false;

	VCreateGameOptions();
	VCreateDatabase();

	WindowOptions& windowOptions = m_pBaseGameOptions->GetWindowsOptions();
	InitInstance(static_cast<int>(windowOptions.m_ScreenSize.x_),
			     static_cast<int>(windowOptions.m_ScreenSize.y_),
				 windowOptions.m_bWindowMode,
				 windowOptions.m_bVSync,
				 windowOptions.m_Multisample,
				 windowOptions.m_bTripleBuffer
				 );
}

bool GameEngineApp::VCreateGameOptions()
{
	m_pBaseGameOptions = new BaseGameOptions();

	return true;
}

bool GameEngineApp::VCreateDatabase()
{
    m_pDatabase = new DatabaseSubsystem(context_);
    return true;
}

void GameEngineApp::Start()
{
    m_pResourceCache = GetSubsystem<ResourceCache>();
    m_pFileSystem = GetSubsystem<FileSystem>();
    if (!m_pBaseGameOptions->InitBaseGameOptions(String("System/GameOptions.xml")))
    {
        URHO3D_LOGERROR("Failed to load simulation options!");
    }

	// Save all necessary subsystems in Application Layer 
	m_pGraphics = GetSubsystem<Graphics>();
	m_pRenderer = GetSubsystem<Renderer>();

	m_pInput = GetSubsystem<Input>();

    m_pDatabase->SetDatabase(GetSubsystem<Database>());

    WindowOptions& windowOptions = m_pBaseGameOptions->GetWindowsOptions();

	m_CurrentWorkDirectory = String::EMPTY;
	m_CurrentWorkDirectory = m_pFileSystem->GetProgramDir();
	m_SaveDirectory = String::EMPTY;
	m_SaveDirectory = m_CurrentWorkDirectory;
	m_SaveDirectory += "GameData/Save";

    if(GetPlatform() == "Android" || GetPlatform() == "iOS")
    {
        // On Android can't set startup resolution as on Windows platform
        windowOptions.m_ScreenSize = IntVector2(m_pGraphics->GetWidth(), m_pGraphics->GetHeight());
    }

	// Load XML file containing default UI style sheet
	XMLFile* style = m_pResourceCache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(style);
    if(!CreateDebugHudAndConsole(style))
    {
        URHO3D_LOGERROR(String("Failed to debug console and debugHud"));
        ErrorDialog("Error", "Failed to debug console and debugHud");
        AbortGame();
    }

	if (!VCreateViewLogic())
	{
		URHO3D_LOGERROR(String("Failed to init game logic"));
		ErrorDialog("Error", "Failed to init game logic");
		AbortGame();
	}

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(GameEngineApp, KeyDownDelegate));
	SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(GameEngineApp, KeyUpDelegate));
#ifdef  ANDROID
    SubscribeToEvent(E_TOUCHBEGIN, URHO3D_HANDLER(GameEngineApp, TouchBeginDelegate));
#endif
	SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(GameEngineApp, MouseMoveDelegate));
    SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(GameEngineApp, MouseDownDelegate));
	SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(GameEngineApp, MouseUpDelegate));
	SubscribeToEvent(E_INPUTFOCUS, URHO3D_HANDLER(GameEngineApp, InputFocusDelegate));
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(GameEngineApp, UpdateDelegate));

#ifndef ANDROID
    if(CreateCursor())
    {
        m_pInput->SetMouseVisible(true);
        m_pCurrentCursor->SetVisible(true);
    }
#endif

    m_bIsInit = true;
	URHO3D_LOGINFO(String("Window width: ") + String(windowOptions.m_ScreenSize.x_) + String(". Window height: ") + String(windowOptions.m_ScreenSize.y_));
}

bool GameEngineApp::CreateDebugHudAndConsole(XMLFile* style)
{
    // Create console
    context_->RegisterSubsystem(new Console(context_));
    // Show the console by default, make it large. Console will show the text edit field when there is at least one
    // subscriber for the console command event
    Console* console = GetSubsystem<Console>();
    console->SetNumRows(GetSubsystem<Graphics>()->GetHeight() / 16);
    console->SetNumBufferedRows(2 * console->GetNumRows());
    console->SetCommandInterpreter(GetTypeName());
    console->SetVisible(false);
    console->GetCloseButton()->SetVisible(false);
    console->SetDefaultStyle(style);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(style);

    if(console && debugHud)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void GameEngineApp::Stop()
{
    if(m_pGameLogic)
    {
        m_pGameLogic->VShutdown();
    }

    g_pAudio->VShutdown();
    SAFE_DELETE(m_pDatabase);
    SAFE_DELETE(m_pBaseGameOptions);
    SAFE_DELETE(g_pAudio);
}

void GameEngineApp::AbortGame()
{
	engine_->Exit();
}

void GameEngineApp::InitInstance(int screenWidth, int screenHeight, bool windowMode, bool vsync, int multisample, bool triplebuffer)
{
	// ScanEngineParameters_ member variable can be modified here	
	if (windowMode)
	{
		engineParameters_["WindowWidth"] = screenWidth;
		engineParameters_["WindowHeight"] = screenHeight;
	}

	//engineParameters_["ResourcePackages"] = String("CoreData.pak");
	engineParameters_["ResourcePaths"] = String("CoreData;Data;GameData");
	engineParameters_["FullScreen"] = !windowMode;
	engineParameters_["WindowIcon"] = GetWindowIcon();
	engineParameters_["WindowTitle"] = GetWindowTitle();
	engineParameters_["WindowResizable"] = false;
	engineParameters_["LogName"] = "logger.txt";
}

bool GameEngineApp::VLoadGame()
{
	// Read the game options and see what the current game
	// needs to be - all of the game graphics are initialized by now, too...
	return m_pGameLogic->VLoadGame(m_pBaseGameOptions->GetSceneName());
}

bool GameEngineApp::OnMessageProc(AppMsg message)
{
	// Always allow dialog resource manager calls to handle global messages
	// so GUI state is updated correctly
	switch (message.uEvent)
	{
		case APP_EVENT::KEY_DOWN:
		case APP_EVENT::KEY_UP:
		case APP_EVENT::MOUSE_MOVE:
		case APP_EVENT::MOUSE_LBUTTONDOWN:
		case APP_EVENT::MOUSE_LBUTTONUP:
		case APP_EVENT::MOUSE_RBUTTONDOWN:
		case APP_EVENT::MOUSE_RBUTTONUP:
        case APP_EVENT::TOUCH_BEGIN:
		{
			GameViewList& gameViews = m_pGameLogic->GetGameViews();

			for (GameViewList::Iterator i = gameViews.Begin(); i != gameViews.End(); ++i)
			{
				if (i->NotNull())
				{
					if ((*i)->VOnMsgProc(message))
					{
						return true;
					}
				}
			}
		}
		break;
	}

	return false;
}

// -----------------------------------------
// Application delegates
// -----------------------------------------
void GameEngineApp::KeyDownDelegate(StringHash eventType, VariantMap& eventData)
{
	using namespace KeyDown;
	// Check for pressing ESC. Note the ScanEngine_ member variable for convenience access to the ScanEngine object
	int key = eventData[P_KEY].GetInt();

	AppMsg msg;
	msg.uEvent = APP_EVENT::KEY_DOWN;
	msg.lParam.m_keycode = key;
	OnMessageProc(msg);
}

void GameEngineApp::MouseMoveDelegate(StringHash eventType, VariantMap& eventData)
{
	AppMsg msg;
	msg.uEvent = MOUSE_MOVE;
	msg.lParam.m_position.x_ = static_cast<float>(eventData[Urho3D::MouseMove::P_X].GetInt());
	msg.lParam.m_position.y_ = static_cast<float>(eventData[Urho3D::MouseMove::P_Y].GetInt());

    m_LastMousePos.x_ = msg.lParam.m_position.x_;
    m_LastMousePos.y_= msg.lParam.m_position.y_;

	msg.lParam.m_delta.x_ = static_cast<float>(eventData[Urho3D::MouseMove::P_DX].GetInt());
	msg.lParam.m_delta.y_ = static_cast<float>(eventData[Urho3D::MouseMove::P_DY].GetInt());

	OnMessageProc(msg);
}

void GameEngineApp::MouseDownDelegate(StringHash eventType, VariantMap& eventData)
{
	AppMsg msg;
	int button = eventData[Urho3D::MouseButtonDown::P_BUTTON].GetInt();
	if (button == Urho3D::MOUSEB_LEFT)
	{
		msg.uEvent = MOUSE_LBUTTONDOWN;
	}
	else
	{
		msg.uEvent = MOUSE_RBUTTONDOWN;
	}

	msg.lParam.m_position.x_ = m_LastMousePos.x_;
	msg.lParam.m_position.y_ = m_LastMousePos.y_;
	
	OnMessageProc(msg);
}

void GameEngineApp::TouchBeginDelegate(StringHash eventType, VariantMap& eventData)
{
    using namespace TouchBegin;

    AppMsg msg;
    msg.uEvent = APP_EVENT::TOUCH_BEGIN;
    msg.lParam.m_position.x_ = eventData[P_X].GetInt();
    msg.lParam.m_position.y_ = eventData[P_Y].GetInt();
    OnMessageProc(msg);
}

void GameEngineApp::MouseUpDelegate(StringHash eventType, VariantMap& eventData)
{
	AppMsg msg;
	int button = eventData[Urho3D::MouseButtonUp::P_BUTTON].GetInt();
	if (button == Urho3D::MOUSEB_LEFT)
	{
		msg.uEvent = MOUSE_LBUTTONUP;
	}
	else
	{
		msg.uEvent = MOUSE_RBUTTONUP;
	}

	msg.lParam.m_position.x_ = m_LastMousePos.x_;
	msg.lParam.m_position.y_ = m_LastMousePos.y_;

	OnMessageProc(msg);
}


void GameEngineApp::InputFocusDelegate(StringHash eventType, VariantMap& eventData)
{
	bool minimized = eventData[Urho3D::InputFocus::P_MINIMIZED].GetBool();
	if (minimized)
	{
		if (m_pGameLogic)
		{
			for (GameViewList::Iterator i = m_pGameLogic->m_GameViews.Begin(); i != m_pGameLogic->m_GameViews.End(); ++i)
			{
				(*i)->VOnLostDevice();
			}
		}
	}
	else
	{
		if (m_pGameLogic)
		{
			for (GameViewList::Iterator i = m_pGameLogic->m_GameViews.Begin(); i != m_pGameLogic->m_GameViews.End(); ++i)
			{
				(*i)->VOnRestore();
			}
		}
	}
}

void GameEngineApp::KeyUpDelegate(StringHash eventType, VariantMap& eventData)
{
	using namespace KeyDown;
	// Check for pressing ESC. Note the ScanEngine_ member variable for convenience access to the ScanEngine object
	int key = eventData[P_KEY].GetInt();

	AppMsg msg;
	msg.uEvent = APP_EVENT::KEY_UP;
	msg.lParam.m_keycode = key;
	OnMessageProc(msg);
}

void GameEngineApp::UpdateDelegate(StringHash eventType, VariantMap& eventData)
{
	float timeStep = eventData[Update::P_TIMESTEP].GetFloat();

	if (m_pGameLogic)
	{
		m_pGameLogic->VOnUpdate(timeStep);
	}
}


DbConnection* GameEngineApp::ConnectToDB(String connectionString)
{
    DbConnection* pConnection = m_pDatabase->Connect(connectionString);
    if (!pConnection)
    {
        URHO3D_LOGERROR("Failed connect to database. Check connection string");
        return NULL;
    }

    return pConnection;
}

bool GameEngineApp::CreateCursor()
{
    Cursor* cursor = new Cursor(context_);
    XMLFile* styles = m_pResourceCache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    cursor->SetDefaultStyle(styles);
    cursor->SetStyleAuto(styles);

    GetSubsystem<UI>()->SetCursor(cursor);

    m_pCurrentCursor = cursor;
    return true;
}
