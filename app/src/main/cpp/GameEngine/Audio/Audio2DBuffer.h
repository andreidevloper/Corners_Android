#pragma once

struct SoundInfo3D;
struct SoundInfo;

class Audio2DBuffer : public IAudioBuffer
{
	URHO3D_OBJECT(Audio2DBuffer, IAudioBuffer)

public:
	Audio2DBuffer(Context* context);
	virtual ~Audio2DBuffer() { }

	virtual void* VGet();
	virtual String VGetName();

	virtual bool VOnRestore();
	virtual bool VLoad2DBuffer(Node* root, const SoundInfo& info);
    virtual bool VLoad3DBuffer(Node* root, const SoundInfo3D& info) { return false; }

    virtual bool VShutdown();

	virtual bool VPlay(float volume, bool looping);
	virtual bool VPause();
	virtual bool VStop();
	virtual bool VResume();

	virtual bool VTogglePause();
	virtual bool VIsPlaying();
	virtual bool VIsLooping() const;
	virtual void VSetVolume(float volume);
	virtual void VSetPosition(signed char* newPosition);
	virtual float VGetVolume() const;
	virtual float VGetProgress();

protected:
	String m_BufferName;
	Sound* m_pSound;
	Node* m_pNode;		
	SoundSource* m_pSoundSource;

	bool m_bIsPaused;
	bool m_bIsBufferLoaded;
	
	volatile signed char* m_Position;
};
