#pragma once

// This interface class describes the public interface for
// a game's audio system.

class IAudioBuffer;

class IAudio : public Object
{
public:
	IAudio(Context* context);

	virtual bool VInitialize() = 0;
	virtual bool VActive() = 0;
	virtual void VShutdown() = 0;

	virtual IAudioBuffer* VInitAudioBuffer(Urho3D::Node* root, String soundName) = 0;
	virtual void VReleaseAudioBuffer(IAudioBuffer* audioBuffer) = 0;
	virtual void VReleaseAudioBuffer(String audioBufferName) = 0;

	virtual void VStopAllSounds() = 0;
	virtual void VPauseAllSounds() = 0;
	virtual void VResumeAllSounds() = 0;

	virtual void VUpdate() = 0;
};

