#include "Track.h"
#include "WaveIn.h"
#include <algorithm>
#include "imgui.h"

float SampleView::operator[](size_t i)
{
	if (i < size)
		return *(start + i);
	else
		return 0.0f;
}

Track::Track(std::string path)
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
		m_Name = name;
		m_SampleRate = wave.format.SamplesPerSec;
		m_SampleCount = wave.data.Subchunk2Size / wave.format.blockAlign;
		for (size_t i = 0; i < wave.format.NumOfChan; i++)
		{
			m_AudioData.emplace_back(m_SampleCount);
		}

		if (wave.format.AudioFormat == 1)
		{//PCM format
			uint32_t bit_mask = 0xFFFFFFFF << (32 - wave.format.bitsPerSample);
			size_t bytes_per_sample = wave.format.bitsPerSample / 8;
			if (bytes_per_sample == 3)
			{//todo support 24 bit pcm
				m_AudioData.clear();
				return;
			}
			for (size_t i = 0; i < wave.data.Subchunk2Size; i += wave.format.blockAlign)
			{
				for (size_t j = 0; j < wave.format.NumOfChan; j++)
				{
					int32_t pcm = bit_mask & *(int32_t*)((char*)wave.data.AudioData + i + j * bytes_per_sample);
					m_AudioData[j][i / wave.format.blockAlign] = (double)pcm / (double)(1ULL << 31);
				}
			}
		}
		else if (wave.format.AudioFormat == 3)
		{//float format
			size_t bytes_per_sample = wave.format.bitsPerSample / 8;
			for (size_t i = 0; i < wave.data.Subchunk2Size; i += wave.format.blockAlign)
				for (size_t j = 0; j < wave.format.NumOfChan; j++)
					m_AudioData[j][i / wave.format.blockAlign] = *(float*)((char*)wave.data.AudioData + i + j * bytes_per_sample);
		}

	}
}

unsigned int Track::GetSampleRate() const
{
	return m_SampleRate;
}

size_t Track::GetSampleCount() const
{
	return m_SampleCount;
}

unsigned int Track::GetChannelCount() const
{
	return m_AudioData.size();
}

SampleView Track::GetSampleView(size_t offset, size_t size, unsigned int channel)
{
	channel = std::clamp(channel, 0U, (unsigned int)m_AudioData.size());

	if (offset > m_SampleCount)
		offset = m_SampleCount;

	size_t end = offset + size;
	if (end >= m_SampleCount)
		size = m_SampleCount - offset;

	return { m_AudioData[channel].data() + offset, size };
}

double Track::GetPosition() const
{
	return m_Position;
}

void Track::AdvancePosition(double delta)
{
	m_Position += delta;
	if (m_Repeat)
		m_Position = fmod(m_Position, (double)m_SampleCount);
	else
	{
		if (m_Position > (double)m_SampleCount)
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

double Track::Gain() const
{
	return pow(10.0, m_Gain / 20.0);
}

bool Track::IsValid() const
{
	return !m_AudioData.empty();
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
		float time_length = (float)(m_SampleCount / m_SampleRate);
		ImGui::SliderFloat("Position", &m_TimePosition, 0.0f, time_length);
		if (ImGui::IsItemDeactivatedAfterEdit())
			m_Position = std::round((double)time_previous * m_SampleRate);

		double min = -60, max = 60;
		ImGui::SliderScalar("Gain(db)", ImGuiDataType_Double, &m_Gain, &min, &max);
	}
	ImGui::End();
	return open;
}
