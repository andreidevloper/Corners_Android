#pragma once

struct SoundInfo3D;
struct SoundInfo;

class Audio3DBuffer : public IAudioBuffer
{
	URHO3D_OBJECT(Audio3DBuffer, IAudioBuffer)

public:
	Audio3DBuffer(Context* context);
	virtual ~Audio3DBuffer() { }

	virtual void* VGet();
	virtual String VGetName();

	virtual bool VOnRestore();
	virtual bool VLoad3DBuffer(Node* root, const SoundInfo3D& info);
    virtual bool VLoad2DBuffer(Node* root, const SoundInfo& info) { return false; }

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

private:
	// Utility function
	void Init3DSource(Urho3D::SoundSource3D* source, const SoundInfo3D& info);

protected:
	String m_BufferName;
	Vector<SharedPtr<Node>> m_Nodes; // all nodes that contains this buffer
	Vector<SoundSource3D*> m_SoundSources;

	Sound* m_pSound;

	bool m_bIsPaused;
	bool m_bIsBufferLoaded;

	volatile signed char* m_Position;
};
