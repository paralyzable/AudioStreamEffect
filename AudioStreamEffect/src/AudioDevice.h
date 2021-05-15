#pragma once
#include <functional>
#include "portaudio.h"

using AudioStream = std::function<void(float* out, size_t pos, size_t count, void* user)>;

class AudioDevice
{
	friend int StreamCallback(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags, void* userData);
private:
	AudioStream m_StreamFunc;
	PaStream* m_PaStream;
	size_t m_Position;
	void* m_UserData;

public:
	AudioDevice(AudioStream func, void* user);
	~AudioDevice();

	void Play();
	void Stop();
};