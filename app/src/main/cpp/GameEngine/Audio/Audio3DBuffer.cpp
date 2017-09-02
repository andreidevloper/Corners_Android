#include "GameEngineStd.h"
#include "Audio3DBuffer.h"
#include "AudioSubsystem.h"

Audio3DBuffer::Audio3DBuffer(Context* context) : IAudioBuffer(context)
{
	m_bIsBufferLoaded = false;
	m_bIsPaused = false;
}

void* Audio3DBuffer::VGet()
{
	return (void*)&m_SoundSources;
}

String Audio3DBuffer::VGetName()
{
	return m_BufferName;
}

bool Audio3DBuffer::VShutdown()
{
	g_pApp->GetResCache()->ReleaseResource(m_pSound->GetType(), m_pSound->GetName());
	for (auto it = m_Nodes.Begin(); it != m_Nodes.End(); it++)
	{
		// if children nodes has the part of the nodename, then create soundsource3d for them too
		(*it)->RemoveComponent<SoundSource3D>();
	}
	m_Nodes.Clear();
	return true;
}

bool Audio3DBuffer::VOnRestore()
{
	VResume();
	return true;
}

bool Audio3DBuffer::VLoad3DBuffer(Node* root, const SoundInfo3D& info)
{
	if (!m_bIsBufferLoaded)
	{
		ResourceCache* cache = g_pApp->GetResCache();

		m_BufferName = info.name;
		m_pSound = cache->GetResource<Sound>(info.soundPath);
		if (!m_pSound)
		{
			return false;
		}

		// if root has the part of the nodename, then create soundsource3d for it
		if (root->GetName().Contains(info.nodeName, false))
		{
			SoundSource3D* newSoundSource = root->CreateComponent<SoundSource3D>();
			Init3DSource(newSoundSource, info);
			m_Nodes.Push(SharedPtr<Node>(root));
		}

		const Urho3D::Vector<SharedPtr<Node>>& childrens = root->GetChildren();

		for (auto it = childrens.Begin(); it != childrens.End(); it++)
		{
			// if children nodes has the part of the nodename, then create soundsource3d for them too
			if ((*it)->GetName().Contains(info.nodeName, false))
			{
				SoundSource3D* newSoundSource = (*it)->CreateComponent<SoundSource3D>();
				Init3DSource(newSoundSource, info);
			}
		}

		m_bIsBufferLoaded = true;
	}

	return true;
}

void Audio3DBuffer::Init3DSource(Urho3D::SoundSource3D* source, const SoundInfo3D& info)
{
	source->SetGain(info.gain);
	source->SetSoundType(info.soundType);

	source->SetNearDistance(info.nearDistance);
	source->SetFarDistance(info.farDistance);
	source->SetDistanceAttenuation(info.nearDistance, info.farDistance, info.rollOffFactor);
	source->SetAngleAttenuation(info.innerAngle, info.outerAngle);

	if (info.autoPlay)
	{
		source->Play(m_pSound);
	}

	m_Position = source->GetPlayPosition();
	m_SoundSources.Push(source);
}

bool Audio3DBuffer::VPlay(float volume, bool looping)
{
	if (m_bIsBufferLoaded)
	{
		if (g_pAudio->VActive())
		{
			m_pSound->SetLooped(looping);
			for (auto it = m_SoundSources.Begin(); it != m_SoundSources.End(); it++)
			{
				(*it)->SetGain(volume);
				(*it)->Play(m_pSound);
			}
			return true;
		}
	}
	return false;
}

bool Audio3DBuffer::VPause()
{
	if (!g_pAudio->VActive())
		return false;

	if (m_bIsBufferLoaded)
	{
		for (auto it = m_SoundSources.Begin(); it != m_SoundSources.End(); it++)
		{
			(*it)->Stop();
			(*it)->SetPlayPosition(0);
		}
		return true;
	}
	return false;
}

bool Audio3DBuffer::VStop()
{
	if (!g_pAudio->VActive())
		return false;

	if (m_bIsBufferLoaded)
	{
		m_bIsPaused = true;
		for (auto it = m_SoundSources.Begin(); it != m_SoundSources.End(); it++)
		{
			(*it)->Stop();
		}
		return true;
	}

	return false;
}

bool Audio3DBuffer::VResume()
{
	m_bIsPaused = false;
	return VPlay(VGetVolume(), VIsLooping());
}

bool Audio3DBuffer::VTogglePause()
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

bool Audio3DBuffer::VIsPlaying()
{
	if (m_bIsBufferLoaded)
	{
		for (auto it = m_SoundSources.Begin(); it != m_SoundSources.End(); it++)
		{
			// if first is playing. then others also are playing
			return (*it)->IsPlaying();
		}
	}

	return false;
}

bool Audio3DBuffer::VIsLooping() const
{
	if (m_bIsBufferLoaded)
	{
		return m_pSound->IsLooped();
	}

	return false;
}

void Audio3DBuffer::VSetVolume(float volume)
{
	if (m_bIsBufferLoaded)
	{
		for (auto it = m_SoundSources.Begin(); it != m_SoundSources.End(); it++)
		{
			return (*it)->SetGain(volume);
		}
	}
}

void Audio3DBuffer::VSetPosition(signed char* newPosition)
{
	if (m_bIsBufferLoaded)
	{
		for (auto it = m_SoundSources.Begin(); it != m_SoundSources.End(); it++)
		{
			return (*it)->SetPlayPosition(newPosition);
		}
	}
}

float Audio3DBuffer::VGetVolume() const
{
	if (m_bIsBufferLoaded)
	{
		for (auto it = m_SoundSources.Begin(); it != m_SoundSources.End(); it++)
		{
			// get gain by first soundsource. other soundsources have the same gain
			return (*it)->GetGain();
		}
	}

	return 0.0f;
}

float Audio3DBuffer::VGetProgress()
{
	if (m_bIsBufferLoaded)
	{
		for (auto it = m_SoundSources.Begin(); it != m_SoundSources.End(); it++)
		{
			// get timeposition by first soundsource. other soundsources have the same timeposition
			return (*it)->GetTimePosition();
		}
	}

	return 0.0f;
}
