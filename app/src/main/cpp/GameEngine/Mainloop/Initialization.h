#pragma once

typedef Vector<String> Resolutions;
typedef Vector<String> Multisamples;
typedef Vector<String> ShadowQualities;
typedef Vector<String> Qualities;
typedef Vector<String> ShadowMapSizes;

struct WindowOptions
{
	WindowOptions() : m_ScreenSize(800,600),
					  m_Position(0,0),
					  m_bWindowMode(true),
					  m_bVSync(false),
					  m_Multisample(2),
					  m_bTripleBuffer(false),
					  m_Renderer("Unknown")
	{

	}

	IntVector2 m_ScreenSize;
	IntVector2 m_Position;
	bool m_bWindowMode;
	bool m_bVSync;
	int m_Multisample;
	bool m_bTripleBuffer;
	String m_Renderer;
};

struct SoundOptions
{
	SoundOptions() : m_bIsEnableSound(true),
					 m_MasterVolume(100),
					 m_SoundEffectsVolume(100),
					 m_MusicVolume(100),
					 m_VoiceVolume(100),
					 m_AmbientVolume(100)
	{

	}

	bool m_bIsEnableSound;
	int m_MasterVolume;
	int m_SoundEffectsVolume;
	int m_MusicVolume;
	int m_VoiceVolume;
	int m_AmbientVolume;
};

struct VideoOptions
{
	VideoOptions() : m_CurrentSelectedRes(0),
					 m_CurrentMultisample(0),
					 m_TextureQuality(0),
					 m_CurrentSelectedTextureQuality(0),
					 m_MaterialQuality(0),
					 m_CurrentSelectedMaterialQuality(0),
					 m_ShadowQuality(SHADOWQUALITY_SIMPLE_16BIT),
					 m_CurrentSelectedShadowQuality(0),
					 m_ShadowMapSize(0),
					 m_CurrrentSelectedShadowMapSize(0),
					 m_bIsDrawShadows(true),
					 m_bIsDynamicInstancing(false),
					 m_bIsSpecularLight(true)
	{

	}

	// Available resolutions
	Resolutions m_Resolutions;
	int m_CurrentSelectedRes;

	// Available multisamples
	Multisamples m_Multisamples;
	int m_CurrentMultisample;

	// All possible qualities for material/texture/shadow
	Qualities m_Qualities;

	// Texture
	int m_TextureQuality;
	int m_CurrentSelectedTextureQuality;

	// Material
	int m_MaterialQuality;
	int m_CurrentSelectedMaterialQuality;

	// Shadows
	ShadowQuality m_ShadowQuality;
	ShadowQualities m_ShadowQualities;
	int m_CurrentSelectedShadowQuality;
	ShadowMapSizes m_ShadowMapSizes;
	int m_ShadowMapSize;
	int m_CurrrentSelectedShadowMapSize;
	bool m_bIsDrawShadows;
	bool m_bIsDynamicInstancing;
	bool m_bIsSpecularLight;
};

struct DebugOptions
{
	DebugOptions() : m_bUseDebugHud(true),
					 m_bUseConsole(true),
					 m_bIsLegacyMode(false),
					 m_bUseDevelopmentDirectories(true)
	{

	}

	bool m_bUseDebugHud;
	bool m_bUseConsole;
	bool m_bIsLegacyMode;
	bool m_bUseDevelopmentDirectories;
};


struct MultiplayerOptions
{
	MultiplayerOptions() : m_MaxAttachedPlayers((int)pow(2, 9)),
						   m_ListenPort(65543),
						   m_GameHost("Host_machine"),
						   m_Login("unknown"),
						   m_Password("unknown"),
						   //m_ConnectionId(0),
						   m_IsEnableMultipayer(true)
	{

	}

	int m_MaxAttachedPlayers;			// server holds it
	int m_ListenPort;
	String m_GameHost;
	String m_Login;
	String m_Password;
	//ConnectionId m_ConnectionId;		// Id that is recieved from database (primary key)
	bool m_IsEnableMultipayer;
};

struct PhysicsOptions
{
	PhysicsOptions() : m_bIsDrawWireFrame(false),
					   m_bIsDrawContactPoints(false),
					   m_bIsDebugPhysics(false)
	{

	}

	// Physics debug
	bool m_bIsDrawWireFrame;
	bool m_bIsDrawContactPoints;
	bool m_bIsDebugPhysics;
};

class BaseGameOptions
{
public:
	BaseGameOptions();
	virtual ~BaseGameOptions();

	virtual bool InitBaseGameOptions(String xmlFilePath);
	bool InitRenderer();

	int GetQuality(String quality);
	String GetQualityStr(int quality);
	ShadowQuality GetShadowQuality(String quality);
	String GetShadowQualityStr(ShadowQuality quality);
	IntVector2 GetResolutionValues(String res);
	Resolutions& GetResolutions() { return m_VideoOptions.m_Resolutions; }

	WindowOptions& GetWindowsOptions() { return m_WindowOptions; }
	VideoOptions& GetVideoOptions() { return m_VideoOptions; }
	SoundOptions& GetSoundOptions() { return m_SoundOptions; }
	DebugOptions& GetDebugOptions() { return m_DebugOptions; }
	PhysicsOptions& GetPhysicsOptions() { return m_PhysicsOptions; }
	const String& GetSceneName() { return m_SceneName; }
	bool IsTouchEnabled() { return m_bIsTouchEnabled; }
    void SetSceneName(String sceneName) { m_SceneName = sceneName; }

protected:
	// Initialization
	void InitResolutions();
	void InitMultisamples();
	void InitShadowQualities();
	void InitQualities();
	void InitShadowMapSizes();
	virtual bool ChangeXML();

protected:
	// XML file
	String m_SimulationOptionFileName;
	SharedPtr<Urho3D::XMLFile> m_pSimulationOptionFile;
	// Window options
	WindowOptions m_WindowOptions;
	// Video options
	VideoOptions m_VideoOptions;
	// PhysicsOptions
	PhysicsOptions m_PhysicsOptions;
	// Sound options
	SoundOptions m_SoundOptions;
	// Debug options
	DebugOptions m_DebugOptions;
	// Mutliplayer options
	MultiplayerOptions m_MultiplayerOptions;
	// Scene options
	String m_SceneName;
	// Is touch enabled
	bool m_bIsTouchEnabled;
};