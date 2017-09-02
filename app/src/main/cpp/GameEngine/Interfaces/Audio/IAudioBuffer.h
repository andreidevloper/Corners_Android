#pragma once

// The interface class that defines the public API for audio buffers.
// An audio buffer maps to one instance of a sound being played,
// which ISNT the sound data. Two different sounds can be played from
// the same source data - such as two explosions in two different places.

struct SoundInfo;
struct SoundInfo3D;

class IAudioBuffer : public Object
{
public:
	IAudioBuffer(Context* context);
	virtual ~IAudioBuffer() { }

	virtual void* VGet() = 0;
	virtual String VGetName() = 0;

	virtual bool VLoad2DBuffer(Node* root, const SoundInfo& info) = 0;
    virtual bool VLoad3DBuffer(Node* root, const SoundInfo3D& info) = 0;

	virtual bool VOnRestore() = 0;
	virtual bool VShutdown() = 0;

	virtual bool VPlay(float volume, bool looping) = 0;
	virtual bool VPause() = 0;
	virtual bool VStop() = 0;
	virtual bool VResume() = 0;

	virtual bool VTogglePause() = 0;
	virtual bool VIsPlaying() = 0;
	virtual bool VIsLooping() const = 0;
	virtual void VSetVolume(float volume) = 0;
	virtual void VSetPosition(signed char* newPosition) = 0;
	virtual float VGetVolume() const = 0;
	virtual float VGetProgress() = 0;
};

