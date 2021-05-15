#include "FilterStreamTransform.h"
#include "imgui.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

void FilterStreamTransform::EndTransform(Track& track, size_t offset, size_t count)
{
	track.AdvancePosition((double)count);
}

std::string FilterStreamTransform::IGetTransformName() const
{
	return std::string("Filter - ") + IGetFilterName();
}

void FilterStreamTransform::ITransformDraw()
{
	unsigned int min = 0, max = 500;
	ImGui::SliderScalar("Kernel size", ImGuiDataType_U32, &m_KernelSize, &min, &max);
	m_KernelSize += m_KernelSize % 2 == 1;
	IFilterDraw();
}

LowPassFST::LowPassFST(const FilterStreamTransform& base)
	:FilterStreamTransform(base)
{
}

SampleView LowPassFST::Transform(Track& track, size_t offset, size_t count, unsigned int channel)
{
	SampleView view = track.GetSampleView(0, track.GetSampleCount(), channel);
	double position = track.GetPosition();

	m_Result[channel].resize(count);
	for (size_t i = 0; i < count; i++)
	{
		int start = (int)std::round(position - m_KernelSize / 2);
		int end = (int)std::round(position + m_KernelSize / 2);
		float result(0.0);
		for (int j = start; j < end; j++)
		{
			float x = (float)(M_PI * (j - position));
			float blackman_window = (0.42f + 0.5f * cos(2.0f * x / m_KernelSize) + 0.08f * cos(4.0f * x / m_KernelSize));
			float zero = x == 0;
			float sinc = sin((float)m_CutoffFraction * x) / (x + zero) * (1.0f - zero) + zero * (float)m_CutoffFraction;
			result += view[(size_t)j] * sinc * blackman_window;
		}
		m_Result[channel][i] = result;
		position += 1.0;
	}

	return { m_Result[channel].data(), count };
}

void LowPassFST::IFilterDraw()
{
	double min = 0.0, max = 1.0;
	ImGui::SliderScalar("Cut-off fraction", ImGuiDataType_Double, &m_CutoffFraction, &min, &max);

	float frequency = (float)m_CutoffFraction * 22050.0f;
	if (ImGui::SliderFloat("Cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f", 2.0f))
		m_CutoffFraction = frequency / 22050.0;
}

HighPassFST::HighPassFST(const FilterStreamTransform& base)
	:FilterStreamTransform(base)
{
}

SampleView HighPassFST::Transform(Track& track, size_t offset, size_t count, unsigned int channel)
{
	SampleView view = track.GetSampleView(0, track.GetSampleCount(), channel);
	double position = track.GetPosition();

	m_Result[channel].resize(count);
	for (size_t i = 0; i < count; i++)
	{
		int start = (int)std::round(position - m_KernelSize / 2);
		int end = (int)std::round(position + m_KernelSize / 2);
		float result(0.0);
		for (int j = start; j < end; j++)
		{
			float x = (float)(M_PI * (j - position));
			float blackman_window = (0.42f + 0.5f * cos(2.0f * x / m_KernelSize) + 0.08f * cos(4.0f * x / m_KernelSize));
			float zero = x == 0;
			float low_sinc = sin((float)m_CutoffFraction * x) / (x + zero) * (1.0f - zero) + zero * (float)m_CutoffFraction;
			float high_sinc = sin(x) / (x + zero) * (1.0f - zero) + zero;
			float sinc = high_sinc - low_sinc;
			result += view[(size_t)j] * sinc * blackman_window;
		}
		m_Result[channel][i] = result;
		position += 1.0;
	}

	return { m_Result[channel].data(), count };
}

void HighPassFST::IFilterDraw()
{
	double min = 0.0, max = 1.0;
	ImGui::SliderScalar("Cut-off fraction", ImGuiDataType_Double, &m_CutoffFraction, &min, &max);
	float frequency = (float)m_CutoffFraction * 22050.0f;
	if (ImGui::SliderFloat("Cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f", 2.0f))
		m_CutoffFraction = frequency / 22050.0;
}

BandPassFST::BandPassFST(const FilterStreamTransform& base)
	:FilterStreamTransform(base)
{
}

SampleView BandPassFST::Transform(Track& track, size_t offset, size_t count, unsigned int channel)
{
	SampleView view = track.GetSampleView(0, track.GetSampleCount(), channel);
	double position = track.GetPosition();

	m_Result[channel].resize(count);
	for (size_t i = 0; i < count; i++)
	{
		int start = (int)std::round(position - m_KernelSize / 2);
		int end = (int)std::round(position + m_KernelSize / 2);
		float result(0.0);
		for (int j = start; j < end; j++)
		{
			float x = (float)(M_PI * (j - position));
			float blackman_window = (0.42f + 0.5f * cos(2.0f * x / m_KernelSize) + 0.08f * cos(4.0f * x / m_KernelSize));
			float zero = x == 0;
			float low_sinc = sin((float)m_CutoffLow * x) / (x + zero) * (1.0f - zero) + zero * (float)m_CutoffLow;
			float high_sinc = sin((float)m_CutoffHigh * x) / (x + zero) * (1.0f - zero) + zero * (float)m_CutoffHigh;
			float sinc = high_sinc - low_sinc;
			result += view[(size_t)j] * sinc * blackman_window;
		}
		m_Result[channel][i] = result;
		position += 1.0;
	}

	return { m_Result[channel].data(), count };
}

void BandPassFST::IFilterDraw()
{
	double min = 0.0, max = 1.0;
	ImGui::SliderScalar("Low cut-off fraction", ImGuiDataType_Double, &m_CutoffLow, &min, &max);
	ImGui::SliderScalar("High cut-off fraction", ImGuiDataType_Double, &m_CutoffHigh, &min, &max);

	float frequency = (float)m_CutoffLow * 22050.0f;
	if (ImGui::SliderFloat("Low cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f", 2.0f))
		m_CutoffLow = frequency / 22050.0;
	frequency = (float)m_CutoffHigh * 22050.0f;
	if (ImGui::SliderFloat("High cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f", 2.0f))
		m_CutoffHigh = frequency / 22050.0;
}