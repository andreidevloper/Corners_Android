#pragma once

struct SoundInfo
{
	String audioData;
	String name;
	String soundDimension;
	String soundPath;
	String soundType;
	float gain;
	bool autoPlay;
};

struct SoundInfo3D : public SoundInfo
{
	String nodeName;
	float nearDistance;
	float farDistance;
	float innerAngle;
	float outerAngle;
	float rollOffFactor;

	SoundInfo3D& operator=(const SoundInfo& info)
	{
		name = info.name;
		soundDimension = info.soundDimension;
		soundPath = info.soundPath;
		soundType = info.soundType;
		gain = info.gain;
		autoPlay = info.autoPlay;
		return *this;
	}
};

class AudioSubsystem : public IAudio
{
	URHO3D_OBJECT(AudioSubsystem, IAudio)

	typedef HashMap<String, SoundInfo> GameSoundsByName;
	typedef HashMap<String, IAudioBuffer*> LoadedSoundsByName;

public:
	AudioSubsystem(Context* context);
	virtual ~AudioSubsystem() { }

	virtual bool VInitialize();
	virtual bool VActive();

	// A bit of an anachronism in name - but it simply returns true
	// if the sound system is active.
	static bool HasSoundCard();

	// unload - true, then unload currently all sounds. and load new one.
	bool LoadGameSoundsXML(String xmlPath, bool unload);
	// Unload buffer, which were loaded from specified audioData
	void UnloadGameSounds(String audioDataName);
	// Unload all buffers
	void UnloadGameSounds();

	virtual IAudioBuffer* VInitAudioBuffer(Node* root, String soundName);
	virtual void VReleaseAudioBuffer(IAudioBuffer* audioBuffer);
	virtual void VReleaseAudioBuffer(String audioBufferName);
	virtual IAudioBuffer* VGetAudioBuffer(const String& bufferName);
	//IAudioBuffer* Play2DBuffer(String name, int volume, bool looping);
	//IAudioBuffer* Play3DBuffer(String name, Node* node, int volume, bool looping);

	// Init, update, shutdown
	virtual void VUpdate() { }
	virtual void VShutdown();

	// Stop all sounds. Will start playing from the beggining.
	virtual void VStopAllSounds();
	// Pause all sounds. Will start from current pause.
	virtual void VPauseAllSounds();
	// Resume/Play all sounds.
	virtual void VResumeAllSounds();

	// Getters/Setters
	bool IsPaused() { return m_bIsAllPaused; }
	bool IsReadyToDelete() { return m_bIsRunning;  }
	bool SetShutDown() { m_bIsRunning = false; return true; }
	Audio* GetAudio() { return m_pAudio; }

protected:
	void HandleSoundEnableDelegate(StringHash eventType, VariantMap& eventData);
	void HandleMasterVolumeDelegate(StringHash eventType, VariantMap& eventData);
	void HandleVoiceVolumeDelegate(StringHash eventType, VariantMap& eventData);
	void HandleEffectVolumeDelegate(StringHash eventType, VariantMap& eventData);
	void HandleAmbientVolumeDelegate(StringHash eventType, VariantMap& eventData);
	void HandleMusicVolumeDelegate(StringHash eventType, VariantMap& eventData);

protected:
	Audio* m_pAudio;
	AudioBufferList m_AllBuffers;							// List of all currently allocated audio buffers

	GameSoundsByName m_AllGameSounds;						// Currently loaded sounds info from xml.
	LoadedSoundsByName m_AllBuffersByName;					// Map of all currently allocated audio buffers

	bool m_bIsInitialized;									// Has the sound system been initialized?
    bool m_bIsAllPaused;									// Has the sound system been paused?
	bool m_bIsRunning;
	XMLFile* m_pGameSounds;									// XML file that stores data about sounds
};

extern AudioSubsystem* g_pAudio;

