#include "AudioDevice.h"

static int StreamCallback(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags, void* userData)
{
	AudioDevice& audio_device = *(AudioDevice*)userData;
	audio_device.m_StreamFunc((float*)output, audio_device.m_Position, frameCount, audio_device.m_UserData);
	audio_device.m_Position += frameCount;
	return paContinue;
}

AudioDevice::AudioDevice(AudioStream func, void* user)
	:m_StreamFunc(func), m_PaStream(nullptr), m_Position(0), m_UserData(user)
{
	Pa_Initialize();
	Pa_OpenDefaultStream(&m_PaStream, 0, 2, paFloat32, 44100, 256, StreamCallback, this);
}

AudioDevice::~AudioDevice()
{
	Pa_Terminate();
}

void AudioDevice::Play()
{
	Pa_StartStream(m_PaStream);
}

void AudioDevice::Stop()
{
	Pa_AbortStream(m_PaStream);
	m_Position = 0;
}
