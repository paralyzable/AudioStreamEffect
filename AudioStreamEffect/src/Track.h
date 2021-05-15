#pragma once
#include <vector>
#include <string>
#include <memory>

struct SampleView
{
	float* start;
	size_t size;

	float operator [](size_t i);
};

class Track
{
private:
	double m_Position = 0;
	bool m_Playing = false;
	bool m_Repeat = false;
	double m_Volume = 1.0;
	float m_TimePosition = 0;

protected:
	unsigned int m_SampleRate = 0;
	double m_Length = 0;
	std::string m_Name;
public:
	Track(unsigned int sample_rate, size_t sample_length, std::string name);
	virtual ~Track() = 0;

	unsigned int GetSampleRate() const;
	virtual size_t GetSampleCount() const = 0;
	virtual unsigned int GetChannelCount() const = 0;
	virtual SampleView GetSampleView(size_t offset, size_t size, unsigned int channel) = 0;
	virtual std::unique_ptr<Track> Clone() const = 0;
	virtual std::unique_ptr<Track> CloneType() const = 0;
	virtual void Resize(size_t size) = 0;
	double GetPosition() const;
	void AdvancePosition(double delta);
	void ResetPosition();
	bool Playing() const;
	double Volume() const;

	bool Draw();

};

std::unique_ptr<Track> OpenTrack(std::string path);

class MonoTrack :public Track
{
private:
	std::vector<float> m_AudioData;

public:
	MonoTrack(std::vector<float>&& audio_data, unsigned int sample_rate, std::string name);

	size_t GetSampleCount() const;
	unsigned int GetChannelCount() const;
	SampleView GetSampleView(size_t offset, size_t size, unsigned int channel);
	std::unique_ptr<Track> Clone() const;
	std::unique_ptr<Track> CloneType() const;
	void Resize(size_t size);
};

class StereoTrack :public Track
{
private:
	std::vector<float> m_AudioLeft;
	std::vector<float> m_AudioRight;

public:
	StereoTrack(std::vector<float>&& audio_left, std::vector<float>&& audio_right, unsigned int sample_rate, std::string name);

	size_t GetSampleCount() const;
	unsigned int GetChannelCount() const;
	SampleView GetSampleView(size_t offset, size_t size, unsigned int channel);
	std::unique_ptr<Track> Clone() const;
	std::unique_ptr<Track> CloneType() const;
	void Resize(size_t size);
};