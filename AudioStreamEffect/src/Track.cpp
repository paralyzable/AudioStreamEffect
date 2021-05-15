#include "Track.h"
#include "WaveIn.h"
#include <algorithm>
#include "imgui.h"

std::unique_ptr<Track> OpenTrack(std::string path)
{
	WAVE wave(path);
	std::string name = path;
	// Remove directory if present.
	const size_t last_slash_idx = name.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
		name.erase(0, last_slash_idx + 1);
	// Remove extension if present.
	const size_t period_idx = name.rfind('.');
	if (std::string::npos != period_idx)
		name.erase(period_idx);

	if (wave.IsValid())
	{
		if (wave.format.NumOfChan == 1)
		{
			size_t pcm_sample_count = wave.data.Subchunk2Size / 2;
			std::vector<float> data(pcm_sample_count);
			for (size_t i = 0; i < pcm_sample_count; i++)
			{
				int16_t pcm = *(int16_t*)((char*)wave.data.AudioData + i * 2);
				data[i] = pcm / 32768.0f;
			}
			return std::make_unique<MonoTrack>(std::move(data), wave.format.SamplesPerSec, name);
		}
		if (wave.format.NumOfChan == 2)
		{
			size_t pcm_sample_count = wave.data.Subchunk2Size / 4;
			std::vector<float> data_left(pcm_sample_count);
			std::vector<float> data_right(pcm_sample_count);
			for (size_t i = 0; i < pcm_sample_count; i++)
			{
				int16_t pcm = *(int16_t*)((char*)wave.data.AudioData + i * 4);
				data_left[i] = pcm / 32768.0f;
				pcm = *(int16_t*)((char*)wave.data.AudioData + i * 4 + 2);
				data_right[i] = pcm / 32768.0f;
			}
			return std::make_unique<StereoTrack>(std::move(data_left), std::move(data_right), wave.format.SamplesPerSec, name);
		}
	}
	return nullptr;
}

Track::Track(unsigned int sample_rate, size_t sample_length, std::string name)
	:m_SampleRate(sample_rate), m_Length((double)sample_length), m_Name(name)
{
}

Track::~Track()
{
}

unsigned int Track::GetSampleRate() const
{
	return m_SampleRate;
}

double Track::GetPosition() const
{
	return m_Position;
}

void Track::AdvancePosition(double delta)
{
	m_Position += delta;
	if (m_Repeat)
		m_Position = fmod(m_Position, m_Length);
	else
	{
		if (m_Position > m_Length)
		{
			m_Playing = false;
			m_Position = 0;
		}
	}
}

void Track::ResetPosition()
{
	m_Position = 0;
}

bool Track::Playing() const
{
	return m_Playing;
}

double Track::Volume() const
{
	return m_Volume;
}

bool Track::Draw()
{
	bool open(true);
	if (ImGui::Begin((std::string("Track: ") + m_Name.c_str() + "###track").c_str(), &open))
	{
		if (ImGui::Button("Play"))
			m_Playing = true;

		ImGui::SameLine();
		if (ImGui::Button("Pause"))
			m_Playing = false;

		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			m_Playing = false;
			ResetPosition();
		}

		ImGui::SameLine();
		ImGui::Checkbox("Repeat", &m_Repeat);

		float time_previous = m_TimePosition;
		m_TimePosition = (float)m_Position / m_SampleRate;
		float time_length = (float)(m_Length / m_SampleRate);
		ImGui::SliderFloat("Position", &m_TimePosition, 0.0f, time_length);
		if (ImGui::IsItemDeactivatedAfterEdit())
			m_Position = std::round((double)time_previous * m_SampleRate);

		double min = 0, max = 2;
		ImGui::SliderScalar("Volume", ImGuiDataType_Double, &m_Volume, &min, &max);
	}
	ImGui::End();
	return open;
}

MonoTrack::MonoTrack(std::vector<float>&& audio_data, unsigned int sample_rate, std::string name)
	:Track(sample_rate, audio_data.size(), name)
{
	m_AudioData = std::move(audio_data);
}

size_t MonoTrack::GetSampleCount() const
{
	return m_AudioData.size();
}

unsigned int MonoTrack::GetChannelCount() const
{
	return 1;
}

SampleView MonoTrack::GetSampleView(size_t offset, size_t size, unsigned int channel)
{
	if (offset > m_AudioData.size())
		offset = m_AudioData.size();

	size_t end = offset + size;
	if (end >= m_AudioData.size())
		size = m_AudioData.size() - offset;

	return { m_AudioData.data() + offset, size };
}

std::unique_ptr<Track> MonoTrack::Clone() const
{
	std::vector<float> audio_data = m_AudioData;
	return std::make_unique<MonoTrack>(std::move(audio_data), m_SampleRate, m_Name);
}

std::unique_ptr<Track> MonoTrack::CloneType() const
{
	std::vector<float> empty;
	return std::make_unique<MonoTrack>(std::move(empty), m_SampleRate, "");
}

void MonoTrack::Resize(size_t size)
{
	m_AudioData.resize(size, 0.0f);
}

StereoTrack::StereoTrack(std::vector<float>&& audio_left, std::vector<float>&& audio_right, unsigned int sample_rate, std::string name)
	:Track(sample_rate, audio_left.size(), name)
{
	m_AudioLeft = std::move(audio_left);
	m_AudioRight = std::move(audio_right);
}

size_t StereoTrack::GetSampleCount() const
{
	return std::max(m_AudioLeft.size(), m_AudioRight.size());
}

unsigned int StereoTrack::GetChannelCount() const
{
	return 2;
}

SampleView StereoTrack::GetSampleView(size_t offset, size_t size, unsigned int channel)
{
	std::vector<float>& audio = channel == 0 ? m_AudioLeft : m_AudioRight;

	if (offset > audio.size())
		offset = audio.size();

	size_t end = offset + size;
	if (end >= audio.size())
		size = audio.size() - offset;

	return { audio.data() + offset, size };
}

std::unique_ptr<Track> StereoTrack::Clone() const
{
	std::vector<float> audio_left = m_AudioLeft;
	std::vector<float> audio_right = m_AudioRight;
	return std::make_unique<StereoTrack>(std::move(audio_left), std::move(audio_right), m_SampleRate, m_Name);
}

std::unique_ptr<Track> StereoTrack::CloneType() const
{
	std::vector<float> empty1;
	std::vector<float> empty2;
	return std::make_unique<StereoTrack>(std::move(empty1), std::move(empty2), m_SampleRate, "");
}

void StereoTrack::Resize(size_t size)
{
	m_AudioLeft.resize(size, 0.0f);
	m_AudioRight.resize(size, 0.0f);
}

float SampleView::operator[](size_t i)
{
	if (i < size)
		return *(start + i);
	else
		return 0.0f;
}
