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
	std::vector<std::vector<float>> m_AudioData;

	unsigned int m_SampleRate = 0;
	size_t m_SampleCount = 0;
	std::string m_Name = "";

	double m_Position = 0;
	bool m_Playing = false;
	bool m_Repeat = false;
	double m_Gain = 0.0;
	float m_TimePosition = 0;

public:
	Track() = default;
	Track(std::string path);

	unsigned int GetSampleRate() const;
	size_t GetSampleCount() const;
	unsigned int GetChannelCount() const;
	SampleView GetSampleView(size_t offset, size_t size, unsigned int channel);
	double GetPosition() const;
	void AdvancePosition(double delta);
	void ResetPosition();
	bool Playing() const;
	double Gain() const;
	bool IsValid() const;

	bool Draw();
};