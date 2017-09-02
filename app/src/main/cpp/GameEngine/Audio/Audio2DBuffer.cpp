#include "GameEngineStd.h"
#include "Audio2DBuffer.h"
#include "AudioSubsystem.h"

Audio2DBuffer::Audio2DBuffer(Context* context) : IAudioBuffer(context)
{
	m_bIsBufferLoaded = false;
	m_bIsPaused = false;
}

void* Audio2DBuffer::VGet()
{
	return (void*)m_pSoundSource;
}

String Audio2DBuffer::VGetName()
{
	return m_BufferName;
}

bool Audio2DBuffer::VOnRestore()
{
	VResume();
	return true;
}

bool Audio2DBuffer::VShutdown()
{
	g_pApp->GetResCache()->ReleaseResource(m_pSound->GetType(), m_pSound->GetName());
	m_pNode->RemoveComponent<SoundSource>();
	return true;
}


bool Audio2DBuffer::VLoad2DBuffer(Node* root, const SoundInfo& info)
{
	if (!m_bIsBufferLoaded)
	{
		ResourceCache* cache = g_pApp->GetResCache();
		m_pSound = cache->GetResource<Sound>(info.soundPath);
		if (!m_pSound)
		{
			return false;
		}

		m_BufferName = info.name;
		m_pNode = root->CreateChild(info.name, CreateMode::LOCAL);
		m_pSoundSource = m_pNode->CreateComponent<SoundSource>();
		m_pSoundSource->SetGain(info.gain);
		m_pSoundSource->SetSoundType(info.soundType);

		if (info.autoPlay)
		{
			m_pSoundSource->Play(m_pSound);
		}

		m_Position = m_pSoundSource->GetPlayPosition();

		m_bIsBufferLoaded = true;
	}

	return true;
}

bool Audio2DBuffer::VPlay(float volume, bool looping)
{
	if (m_bIsBufferLoaded)
	{
		if (g_pAudio->VActive())
		{
			m_pSoundSource->SetGain(volume);
			m_pSound->SetLooped(looping);

			m_pSoundSource->Play(m_pSound);
			return true;
		}
	}
	return false;
}

bool Audio2DBuffer::VPause()
{
	if (!g_pAudio->VActive())
		return false;

	if (m_bIsBufferLoaded)
	{
		m_pSoundSource->Stop();
		m_pSoundSource->SetPlayPosition(0);
	}

	return true;
}

bool Audio2DBuffer::VStop()
{
	if (!g_pAudio->VActive())
		return false;

	if (m_bIsBufferLoaded)
	{
		m_bIsPaused = true;

		m_pSoundSource->Stop();
	}
	return true;
}

bool Audio2DBuffer::VResume()
{
	m_bIsPaused = false;
	return VPlay(VGetVolume(), VIsLooping());
}

bool Audio2DBuffer::VTogglePause()
{
	if (!g_pAudio->VActive())
		return false;

	if (m_bIsPaused)
	{
		return VResume();
	}
	else
	{
		return VPause();
	}

	return false;
}

bool Audio2DBuffer::VIsPlaying()
{
	if (m_bIsBufferLoaded)
	{
		return m_pSoundSource->IsPlaying();
	}

	return false;
}

bool Audio2DBuffer::VIsLooping() const
{
	if (m_bIsBufferLoaded)
	{
		return m_pSound->IsLooped();
	}

	return false;
}

void Audio2DBuffer::VSetVolume(float volume)
{
	if (m_bIsBufferLoaded)
	{
		m_pSoundSource->SetGain(volume);
	}
}

void Audio2DBuffer::VSetPosition(signed char* newPosition)
{
	if (m_bIsBufferLoaded)
	{
		m_pSoundSource->SetPlayPosition(newPosition);
	}
}

float Audio2DBuffer::VGetVolume() const
{
	if (m_bIsBufferLoaded)
	{
		return m_pSoundSource->GetGain();
	}

	return 0.0f;
}

float Audio2DBuffer::VGetProgress()
{
	if (m_bIsBufferLoaded)
	{
		return m_pSoundSource->GetTimePosition();
	}

	return 0.0f;
}
