#include "FilterStreamTransform.h"
#include "imgui.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

FilterStreamTransform::FilterStreamTransform()
{
	RegenerateCache();
}

FilterStreamTransform::FilterStreamTransform(const FilterStreamTransform& other)
	:m_KernelSize(other.m_KernelSize), m_Window(CreateWindowFromID(other.m_Window->ID())), m_WindowCache(other.m_WindowCache)
{
}

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
	bool kernel_update(false);
	float size = m_KernelSize;
	if (ImGui::SliderFloat("Kernel size", &size, 1, 20001, "%.0f", ImGuiSliderFlags_Logarithmic))
	{
		m_KernelSize = (int)std::round(size);
		kernel_update = true;
	}
	m_KernelSize += m_KernelSize % 2 == 0;

	int current = m_Window->ID();
	bool window_update(false);
	if (ImGui::Combo("Window", &current, windows_list, std::size(windows_list)))
	{
		window_update = true;
		m_Window = CreateWindowFromID(current);
	}
	m_Window->Draw();
	if (kernel_update || window_update)
		RegenerateCache();

	IFilterDraw(kernel_update);
} 

void FilterStreamTransform::RegenerateCache()
{
	m_WindowCache.resize(m_KernelSize);
	int L = ((int)m_KernelSize - 1) / 2;
	for (int x = -L; x <= L; x++)
	{
		m_WindowCache[x + L] = (*m_Window)(x, m_KernelSize);
	}
}

LowPassFST::LowPassFST()
{
	RegenerateCache();
}

LowPassFST::LowPassFST(const FilterStreamTransform& base)
	:FilterStreamTransform(base)
{
	RegenerateCache();
}

SampleView LowPassFST::Transform(Track& track, size_t offset, size_t count, unsigned int channel)
{
	SampleView view = track.GetSampleView(0, track.GetSampleCount(), channel);
	uint64_t position = (uint64_t)std::round(track.GetPosition());

	m_Result[channel].resize(count);
	for (size_t i = 0; i < count; i++)
	{
		int L = ((int)m_KernelSize - 1) / 2;
		float result(0.0);
		for (int n = 0; n < m_KernelSize; n++)
			result += view[(size_t)(position + n - L)] * m_SincCache[n] * m_WindowCache[n];
		m_Result[channel][i] = result;
		position += 1.0;
	}

	return { m_Result[channel].data(), count };
}

void LowPassFST::IFilterDraw(bool kernel_update)
{
	float frequency = (float)m_CutoffFraction * 22050.0f;
	if (ImGui::SliderFloat("Cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f",ImGuiSliderFlags_Logarithmic))
	{
		m_CutoffFraction = frequency / 22050.0;
		kernel_update = true;
	}

	if (kernel_update)
		RegenerateCache();
}

void LowPassFST::RegenerateCache()
{
	m_SincCache.resize(m_KernelSize);
	int L = ((int)m_KernelSize - 1) / 2;
	for (int x = -L; x <= L; x++)
	{
		float zero = x == 0;
		m_SincCache[x + L] = sin(M_PI * (float)m_CutoffFraction * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffFraction;
	}
}

HighPassFST::HighPassFST()
{
	RegenerateCache();
}

HighPassFST::HighPassFST(const FilterStreamTransform& base)
	:FilterStreamTransform(base)
{
	RegenerateCache();
}

SampleView HighPassFST::Transform(Track& track, size_t offset, size_t count, unsigned int channel)
{
	SampleView view = track.GetSampleView(0, track.GetSampleCount(), channel);
	uint64_t position = (uint64_t)std::round(track.GetPosition());

	m_Result[channel].resize(count);
	for (size_t i = 0; i < count; i++)
	{
		int L = ((int)m_KernelSize - 1) / 2;
		float result(0.0);
		for (int n = 0; n < m_KernelSize; n++)
			result += view[(size_t)(position + n - L)] * m_SincCache[n] * m_WindowCache[n];
		m_Result[channel][i] = result;
		position += 1.0;
	}

	return { m_Result[channel].data(), count };
}

void HighPassFST::IFilterDraw(bool kernel_update)
{
	float frequency = (float)m_CutoffFraction * 22050.0f;
	if (ImGui::SliderFloat("Cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
	{
		m_CutoffFraction = frequency / 22050.0;
		kernel_update = true;
	}

	if (kernel_update)
		RegenerateCache();
}

void HighPassFST::RegenerateCache()
{
	m_SincCache.resize(m_KernelSize);
	int L = ((int)m_KernelSize - 1) / 2;
	for (int x = -L; x <= L; x++)
	{
		float zero = x == 0;
		m_SincCache[x + L] = zero-(sin(M_PI * (float)m_CutoffFraction * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffFraction);
	}
}

BandPassFST::BandPassFST()
{
	RegenerateCache();
}

BandPassFST::BandPassFST(const FilterStreamTransform& base)
	:FilterStreamTransform(base)
{
	RegenerateCache();
}

SampleView BandPassFST::Transform(Track& track, size_t offset, size_t count, unsigned int channel)
{
	SampleView view = track.GetSampleView(0, track.GetSampleCount(), channel);
	uint64_t position = (uint64_t)std::round(track.GetPosition());

	m_Result[channel].resize(count);
	for (size_t i = 0; i < count; i++)
	{
		int L = ((int)m_KernelSize - 1) / 2;
		float result(0.0);
		for (int n = 0; n < m_KernelSize; n++)
			result += view[(size_t)(position + n - L)] * m_SincCache[n] * m_WindowCache[n];
		m_Result[channel][i] = result;
		position += 1.0;
	}

	return { m_Result[channel].data(), count };
}

void BandPassFST::IFilterDraw(bool kernel_update)
{
	float frequency = (float)m_CutoffLow * 22050.0f;
	if (ImGui::SliderFloat("Low cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
	{
		m_CutoffLow = frequency / 22050.0;
		kernel_update = true;
	}
	frequency = (float)m_CutoffHigh * 22050.0f;
	if (ImGui::SliderFloat("High cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
	{
		m_CutoffHigh = frequency / 22050.0;
		kernel_update = true;
	}

	if (kernel_update)
		RegenerateCache();
}

void BandPassFST::RegenerateCache()
{
	m_SincCache.resize(m_KernelSize);
	int L = ((int)m_KernelSize - 1) / 2;
	for (int x = -L; x <= L; x++)
	{
		float zero = x == 0;
		float low = sin(M_PI * (float)m_CutoffLow * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffLow;
		float high = sin(M_PI * (float)m_CutoffHigh * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffHigh;
		m_SincCache[x + L] = high - low;
	}
}