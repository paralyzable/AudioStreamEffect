#include "Filters.h"
#include "imgui.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "..\frequency_domain\Filters.h"

LowPassTF::LowPassTF()
{
	RegenerateCache();
}

LowPassTF::LowPassTF(TimeDomainFilter&& base)
	:TimeDomainFilter(std::move(base))
{
	RegenerateCache();
}

std::string LowPassTF::IGetFilterName() const
{
	return "Low pass";
}

bool LowPassTF::IFilterDraw(bool kernel_update)
{
	float frequency = (float)m_CutoffFraction * 22050.0f;
	if (ImGui::SliderFloat("Cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
	{
		m_CutoffFraction = frequency / 22050.0;
		kernel_update = true;
	}

	if (kernel_update)
		RegenerateCache();

	return kernel_update;
}

void LowPassTF::RegenerateCache()
{
	m_KernelCache.resize(m_KernelSize);
	int L = ((int)m_KernelSize - 1) / 2;
	for (int x = -L; x <= L; x++)
	{
		float zero = x == 0;
		m_KernelCache[x + L] = sin(M_PI * (float)m_CutoffFraction * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffFraction;
	}
}

HighPassTF::HighPassTF()
{
	RegenerateCache();
}

HighPassTF::HighPassTF(TimeDomainFilter&& base)
	:TimeDomainFilter(std::move(base))
{
	RegenerateCache();
}

std::string HighPassTF::IGetFilterName() const
{
	return "High pass";
}

bool HighPassTF::IFilterDraw(bool kernel_update)
{
	float frequency = (float)m_CutoffFraction * 22050.0f;
	if (ImGui::SliderFloat("Cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
	{
		m_CutoffFraction = frequency / 22050.0;
		kernel_update = true;
	}

	if (kernel_update)
		RegenerateCache();

	return kernel_update;
}

void HighPassTF::RegenerateCache()
{
	m_KernelCache.resize(m_KernelSize);
	int L = ((int)m_KernelSize - 1) / 2;
	for (int x = -L; x <= L; x++)
	{
		float zero = x == 0;
		m_KernelCache[x + L] = zero - (sin(M_PI * (float)m_CutoffFraction * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffFraction);
	}
}

BandPassTF::BandPassTF()
{
	RegenerateCache();
}

BandPassTF::BandPassTF(TimeDomainFilter&& base)
	:TimeDomainFilter(std::move(base))
{
	RegenerateCache();
}

std::string BandPassTF::IGetFilterName() const
{
	return "Band pass";
}

bool BandPassTF::IFilterDraw(bool kernel_update)
{
	float frequency = (float)m_CutoffLow * 22050.0f;
	if (ImGui::SliderFloat("Low cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
	{
		m_CutoffLow = frequency / 22050.0;
		if (m_CutoffLow > m_CutoffHigh)
			m_CutoffHigh = m_CutoffLow;
		kernel_update = true;
	}
	frequency = (float)m_CutoffHigh * 22050.0f;
	if (ImGui::SliderFloat("High cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
	{
		m_CutoffHigh = frequency / 22050.0;
		if (m_CutoffHigh < m_CutoffLow)
			m_CutoffLow = m_CutoffHigh;
		kernel_update = true;
	}

	if (kernel_update)
		RegenerateCache();

	return kernel_update;
}

void BandPassTF::RegenerateCache()
{
	m_KernelCache.resize(m_KernelSize);
	int L = ((int)m_KernelSize - 1) / 2;
	for (int x = -L; x <= L; x++)
	{
		float zero = x == 0;
		float low = sin(M_PI * (float)m_CutoffLow * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffLow;
		float high = sin(M_PI * (float)m_CutoffHigh * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffHigh;
		m_KernelCache[x + L] = high - low;
	}
}


BandRejectTF::BandRejectTF()
{
	RegenerateCache();
}

BandRejectTF::BandRejectTF(TimeDomainFilter&& base)
	:TimeDomainFilter(std::move(base))
{
	RegenerateCache();
}

std::string BandRejectTF::IGetFilterName() const
{
	return "Band reject";
}

bool BandRejectTF::IFilterDraw(bool kernel_update)
{
	float frequency = (float)m_CutoffLow * 22050.0f;
	if (ImGui::SliderFloat("Low cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
	{
		m_CutoffLow = frequency / 22050.0;
		if (m_CutoffLow > m_CutoffHigh)
			m_CutoffHigh = m_CutoffLow;
		kernel_update = true;
	}
	frequency = (float)m_CutoffHigh * 22050.0f;
	if (ImGui::SliderFloat("High cut-off frequency(log)", &frequency, 20.0f, 22050.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
	{
		m_CutoffHigh = frequency / 22050.0;
		if (m_CutoffHigh < m_CutoffLow)
			m_CutoffLow = m_CutoffHigh;
		kernel_update = true;
	}

	if (kernel_update)
		RegenerateCache();

	return kernel_update;
}

void BandRejectTF::RegenerateCache()
{
	m_KernelCache.resize(m_KernelSize);
	int L = ((int)m_KernelSize - 1) / 2;
	for (int x = -L; x <= L; x++)
	{
		float zero = x == 0;
		float low = sin(M_PI * (float)m_CutoffLow * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffLow;
		float high = zero - (sin(M_PI * (float)m_CutoffHigh * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffHigh);
		m_KernelCache[x + L] = high + low;
	}
}
