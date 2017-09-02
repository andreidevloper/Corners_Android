#include "GameEngineStd.h"

#include "Events/GameEngineEvents.h"

#include "AudioSubsystem.h"
#include "Audio2DBuffer.h"
#include "Audio3DBuffer.h"

AudioSubsystem* g_pAudio = nullptr;

AudioSubsystem::AudioSubsystem(Context* context) : IAudio(context), m_bIsInitialized(false), m_bIsAllPaused(false)
{

}

bool AudioSubsystem::VInitialize()
{
	if (!m_bIsInitialized)
	{
		m_pAudio = GetSubsystem<Audio>();

		SubscribeToEvent(EVENT_DATA_ENABLE_SOUND, URHO3D_HANDLER(AudioSubsystem, HandleSoundEnableDelegate));
		SubscribeToEvent(EVENT_DATA_MASTER_VOLUME, URHO3D_HANDLER(AudioSubsystem, HandleMasterVolumeDelegate));
		SubscribeToEvent(EVENT_DATA_VOICE_VOLUME, URHO3D_HANDLER(AudioSubsystem, HandleVoiceVolumeDelegate));
		SubscribeToEvent(EVENT_DATA_SFX_VOLUME, URHO3D_HANDLER(AudioSubsystem, HandleEffectVolumeDelegate));
		SubscribeToEvent(EVENT_DATA_AMBIENT_VOLUME, URHO3D_HANDLER(AudioSubsystem, HandleAmbientVolumeDelegate));
		SubscribeToEvent(EVENT_DATA_MUSIC_VOLUME, URHO3D_HANDLER(AudioSubsystem, HandleMusicVolumeDelegate));
	}

	BaseGameOptions* option = g_pApp->GetGameOptions();
	SoundOptions& soundOptions = option->GetSoundOptions();
    
	if (!soundOptions.m_bIsEnableSound)
	{
		soundOptions.m_MasterVolume = 0;
	}
	m_pAudio->SetMasterGain(SOUND_MASTER, soundOptions.m_MasterVolume / 100.0f);
	m_pAudio->SetMasterGain(SOUND_EFFECT, soundOptions.m_SoundEffectsVolume / 100.0f);
	m_pAudio->SetMasterGain(SOUND_MUSIC, soundOptions.m_MusicVolume / 100.0f);
	m_pAudio->SetMasterGain(SOUND_VOICE, soundOptions.m_VoiceVolume / 100.0f);
	m_pAudio->SetMasterGain(SOUND_AMBIENT, soundOptions.m_AmbientVolume / 100.0f);

	m_bIsInitialized = true;

	return true;
}

bool AudioSubsystem::VActive()
{
	return m_pAudio ? true : false;
}


bool AudioSubsystem::LoadGameSoundsXML(String xmlPath, bool unload)
{
	// first unload all currently loaded resources
	if (unload)
	{
		UnloadGameSounds();
	}

	XMLFile* file = g_pApp->GetResCache()->GetResource<XMLFile>(xmlPath);

	if (!file)
	{
		URHO3D_LOGERROR("Failed to load game sounds data from xml file");
		return false;
	}

	XMLElement root = file->GetRoot();
	if (root.GetName() == "AudioData")
	{
		String audioDataName = root.GetAttribute("name");
		for (auto it = root.GetChild(); it; it = it.GetNext())
		{
			String audioName = it.GetAttribute("name");
			String dimension = it.GetAttribute("dimension");
			String type = it.GetAttribute("type");
			String path = it.GetAttribute("path");
			float defaultGain = it.GetFloat("gain");
			bool autoPlay = it.GetBool("autoPlay");

			if (m_AllGameSounds.Contains(audioName))
			{
				URHO3D_LOGERROR("Warning! Sound info overwriting !");
			}

			SoundInfo newSound;
			newSound.audioData = audioDataName;
			newSound.name = audioName;
			newSound.soundPath = path;
			newSound.soundType = type;
			newSound.gain = defaultGain;
			newSound.autoPlay = autoPlay;
			newSound.soundDimension = dimension;

			m_AllGameSounds[audioName] = newSound;

			SoundInfo3D newSound3D;
			if (it.GetName().Contains("3D", false))
			{
				XMLElement innerChild = it.GetChild();
				newSound3D = newSound;

				String nodeName = innerChild.GetAttribute("value");
				innerChild = innerChild.GetNext();
				float nearDistance = innerChild.GetFloat("value");
				innerChild = innerChild.GetNext();
				float farDistance = innerChild.GetFloat("value");
				innerChild = innerChild.GetNext();
				float innerAngle = innerChild.GetFloat("value");
				innerChild = innerChild.GetNext();
				float outerAngle = innerChild.GetFloat("value");
				innerChild = innerChild.GetNext();
				float rollOffFactor = innerChild.GetFloat("value");
				innerChild = innerChild.GetNext();

				newSound3D.nearDistance = nearDistance;
				newSound3D.farDistance = farDistance;
				newSound3D.innerAngle = innerAngle;
				newSound3D.outerAngle = outerAngle;
				newSound3D.rollOffFactor = rollOffFactor;

				m_AllGameSounds[audioName] = newSound3D;
			}
		}
	}
	return true;
}

void AudioSubsystem::UnloadGameSounds(String audioDataName)
{
	auto it = m_AllGameSounds.Begin();
	while(it != m_AllGameSounds.End())
	{
		if ((*it).second_.audioData == audioDataName)
		{
			String soundName = (*it).second_.name;
			if (m_AllBuffersByName.Contains(soundName))
			{
				IAudioBuffer* buffer = m_AllBuffersByName[soundName];
				buffer->VStop();
				buffer->VShutdown();
				SAFE_DELETE(buffer);
				m_AllBuffersByName.Erase(soundName);
			}
			it = m_AllGameSounds.Erase(it);
		}
		else
		{
			it++;
		}
	}

}

void AudioSubsystem::UnloadGameSounds()
{
	m_AllGameSounds.Clear();

	for (auto it = m_AllBuffersByName.Begin(); it != m_AllBuffersByName.End(); it++)
	{
		it->second_->VStop();
		it->second_->VShutdown();
		SAFE_DELETE(it->second_);
	}

	m_AllBuffersByName.Clear();
}

IAudioBuffer* AudioSubsystem::VInitAudioBuffer(Node* root, String soundName)
{
	ResourceCache* cache = g_pApp->GetResCache();

	if (m_AllBuffersByName.Contains(soundName))
	{
		return m_AllBuffersByName[soundName];
	}
	else
	{
		SoundInfo& info = m_AllGameSounds[soundName];

		// Create buffer
		if (info.soundDimension == "2D")
		{
			IAudioBuffer* newBuffer = new Audio2DBuffer(context_);
			if (newBuffer->VLoad2DBuffer(root, info))
			{
				m_AllBuffersByName[info.name] = newBuffer;
			}
			else
			{
				SAFE_DELETE(newBuffer);
				URHO3D_LOGERROR("Failed to init 2D sound buffer: " + info.name);
				return NULL;
			}

			return newBuffer;
		}
		else if (info.soundDimension == "3D")
		{
			IAudioBuffer* newBuffer = new Audio3DBuffer(context_);

            SoundInfo3D soundInfo3D = (SoundInfo3D&)info;
			if (newBuffer->VLoad3DBuffer(root, soundInfo3D))
			{
				m_AllBuffersByName[info.name] = newBuffer;
			}
			else
			{
				SAFE_DELETE(newBuffer);
                URHO3D_LOGERROR("Failed to init 3D sound buffer: " + info.name);
				return NULL;
			}

			return newBuffer;
		}
	}

	return nullptr;
}

void AudioSubsystem::VReleaseAudioBuffer(String audioBufferName)
{
	if (m_AllBuffersByName.Contains(audioBufferName))
	{
		VReleaseAudioBuffer(m_AllBuffersByName[audioBufferName]);
	}
}

IAudioBuffer * AudioSubsystem::VGetAudioBuffer(const String & bufferName)
{
	IAudioBuffer* result = NULL;
	m_AllBuffersByName.TryGetValue(bufferName, result);
	return result;
}

void AudioSubsystem::VReleaseAudioBuffer(IAudioBuffer* audioBuffer)
{
	audioBuffer->VStop();

	String bufferName = audioBuffer->VGetName();
	m_AllBuffersByName.Erase(bufferName);
	audioBuffer->VShutdown();

	SAFE_DELETE(audioBuffer);
}

void AudioSubsystem::VShutdown()
{
	LoadedSoundsByName::Iterator it = m_AllBuffersByName.Begin();

	while (it != m_AllBuffersByName.End())
	{
		IAudioBuffer* pAudioSubsystemBuffer = it->second_;
		if (pAudioSubsystemBuffer)
		{
			pAudioSubsystemBuffer->VStop();
			SAFE_DELETE(pAudioSubsystemBuffer);
		}
		it++;
	}
	m_AllBuffersByName.Clear();
}

void AudioSubsystem::VStopAllSounds()
{
	IAudioBuffer *AudioSubsystemBuffer = nullptr;

	AudioBufferList::Iterator i;
	AudioBufferList::Iterator end;
	for (i = m_AllBuffers.Begin(), end = m_AllBuffers.End(); i != end; ++i)
	{
		AudioSubsystemBuffer = (*i);
		AudioSubsystemBuffer->VStop();
	}

	m_bIsAllPaused = false;
}


void AudioSubsystem::VPauseAllSounds()
{
	AudioBufferList::Iterator i;
	AudioBufferList::Iterator end;
	for (i = m_AllBuffers.Begin(), end = m_AllBuffers.End(); i != end; ++i)
	{
		IAudioBuffer *AudioSubsystemBuffer = (*i);
		AudioSubsystemBuffer->VPause();
	}

	m_bIsAllPaused = true;
}


void AudioSubsystem::VResumeAllSounds()
{
	AudioBufferList::Iterator i;
	AudioBufferList::Iterator end;
	for (i = m_AllBuffers.Begin(), end = m_AllBuffers.End(); i != end; ++i)
	{
		IAudioBuffer *AudioSubsystemBuffer = (*i);
		AudioSubsystemBuffer->VResume();
	}

	m_bIsAllPaused = false;
}

bool AudioSubsystem::HasSoundCard()
{
	return (g_pApp->GetContext()->GetSubsystem<AudioSubsystem>() && g_pApp->GetContext()->GetSubsystem<AudioSubsystem>()->VActive());
}


void AudioSubsystem::HandleSoundEnableDelegate(StringHash eventType, VariantMap& eventData)
{
	bool value = g_pApp->GetGameOptions()->GetSoundOptions().m_bIsEnableSound;
	if (!value)
	{
		m_pAudio->SetMasterGain(SOUND_MASTER, 0.0f);
	}
	else
	{
		m_pAudio->SetMasterGain(SOUND_MASTER, g_pApp->GetGameOptions()->GetSoundOptions().m_MasterVolume / 100.0f);
	}
}

void AudioSubsystem::HandleMasterVolumeDelegate(StringHash eventType, VariantMap& eventData)
{
	m_pAudio->SetMasterGain(SOUND_MASTER, g_pApp->GetGameOptions()->GetSoundOptions().m_MasterVolume / 100.0f);
}

void AudioSubsystem::HandleVoiceVolumeDelegate(StringHash eventType, VariantMap& eventData)
{
	m_pAudio->SetMasterGain(SOUND_VOICE, g_pApp->GetGameOptions()->GetSoundOptions().m_VoiceVolume / 100.0f);
}

void AudioSubsystem::HandleEffectVolumeDelegate(StringHash eventType, VariantMap& eventData)
{
	m_pAudio->SetMasterGain(SOUND_EFFECT, g_pApp->GetGameOptions()->GetSoundOptions().m_SoundEffectsVolume / 100.0f);
}

void AudioSubsystem::HandleAmbientVolumeDelegate(StringHash eventType, VariantMap& eventData)
{
	m_pAudio->SetMasterGain(SOUND_AMBIENT, g_pApp->GetGameOptions()->GetSoundOptions().m_AmbientVolume / 100.0f);
}

void AudioSubsystem::HandleMusicVolumeDelegate(StringHash eventType, VariantMap& eventData)
{
	m_pAudio->SetMasterGain(SOUND_MUSIC, g_pApp->GetGameOptions()->GetSoundOptions().m_MusicVolume / 100.0f);
}


