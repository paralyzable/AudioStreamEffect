#include "Filters.h"
#include "FastFourierTransform.h"
#include "imgui.h"

LowPassFF::LowPassFF()
{
	RegenerateCache();
}

LowPassFF::LowPassFF(FrequencyDomainFilter&& base)
	:FrequencyDomainFilter(std::move(base))
{
	RegenerateCache();
}

std::string LowPassFF::IGetFilterName() const
{
	return "Low pass";
}

bool LowPassFF::IFilterDraw(bool kernel_update)
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

void LowPassFF::RegenerateCache()
{
	std::fill(m_FrequencyCacheReal.begin(), m_FrequencyCacheReal.end(), 0.0f);
	std::fill(m_FrequencyCacheImg.begin(), m_FrequencyCacheImg.end(), 0.0f);

	int L = ((int)m_KernelSize - 1) / 2;
	for (int x = -L; x <= L; x++)
	{
		float zero = x == 0;
		m_FrequencyCacheReal[x + L] = sin(M_PI * (float)m_CutoffFraction * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffFraction;
		m_FrequencyCacheReal[x + L] *= (*m_Window)(x, m_KernelSize);
	}
	FFT(m_FrequencyCacheReal, m_FrequencyCacheImg, m_TotalSizeLog2);
}

HighPassFF::HighPassFF()
{
	RegenerateCache();
}

HighPassFF::HighPassFF(FrequencyDomainFilter&& base)
	:FrequencyDomainFilter(std::move(base))
{
	RegenerateCache();
}


std::string HighPassFF::IGetFilterName() const
{
	return "High pass";
}

bool HighPassFF::IFilterDraw(bool kernel_update)
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

void HighPassFF::RegenerateCache()
{
	std::fill(m_FrequencyCacheReal.begin(), m_FrequencyCacheReal.end(), 0.0f);
	std::fill(m_FrequencyCacheImg.begin(), m_FrequencyCacheImg.end(), 0.0f);

	int L = ((int)m_KernelSize - 1) / 2;
	for (int x = -L; x <= L; x++)
	{
		float zero = x == 0;
		m_FrequencyCacheReal[x + L] = zero - (sin(M_PI * (float)m_CutoffFraction * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffFraction);
		m_FrequencyCacheReal[x + L] *= (*m_Window)(x, m_KernelSize);
	}
	FFT(m_FrequencyCacheReal, m_FrequencyCacheImg, m_TotalSizeLog2);
}

BandPassFF::BandPassFF()
{
	RegenerateCache();
}

BandPassFF::BandPassFF(FrequencyDomainFilter&& base)
	:FrequencyDomainFilter(std::move(base))
{
	RegenerateCache();
}


std::string BandPassFF::IGetFilterName() const
{
	return "Band pass";
}

bool BandPassFF::IFilterDraw(bool kernel_update)
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

void BandPassFF::RegenerateCache()
{
	std::fill(m_FrequencyCacheReal.begin(), m_FrequencyCacheReal.end(), 0.0f);
	std::fill(m_FrequencyCacheImg.begin(), m_FrequencyCacheImg.end(), 0.0f);

	int L = ((int)m_KernelSize - 1) / 2;
	for (int x = -L; x <= L; x++)
	{
		float zero = x == 0;
		float low = sin(M_PI * (float)m_CutoffLow * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffLow;
		float high = sin(M_PI * (float)m_CutoffHigh * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffHigh;
		m_FrequencyCacheReal[x + L] = (high - low) * (*m_Window)(x, m_KernelSize);
	}
	FFT(m_FrequencyCacheReal, m_FrequencyCacheImg, m_TotalSizeLog2);
}

BandRejectFF::BandRejectFF()
{
	RegenerateCache();
}

BandRejectFF::BandRejectFF(FrequencyDomainFilter&& base)
	:FrequencyDomainFilter(std::move(base))
{
	RegenerateCache();
}


std::string BandRejectFF::IGetFilterName() const
{
	return "Band reject";
}

bool BandRejectFF::IFilterDraw(bool kernel_update)
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

void BandRejectFF::RegenerateCache()
{
	std::fill(m_FrequencyCacheReal.begin(), m_FrequencyCacheReal.end(), 0.0f);
	std::fill(m_FrequencyCacheImg.begin(), m_FrequencyCacheImg.end(), 0.0f);

	int L = ((int)m_KernelSize - 1) / 2;
	for (int x = -L; x <= L; x++)
	{
		float zero = x == 0;
		float low = sin(M_PI * (float)m_CutoffLow * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffLow;
		float high = zero - (sin(M_PI * (float)m_CutoffHigh * x) / (M_PI * x + zero) * (1.0f - zero) + zero * (float)m_CutoffHigh);
		m_FrequencyCacheReal[x + L] = (high + low) * (*m_Window)(x, m_KernelSize);
	}
	FFT(m_FrequencyCacheReal, m_FrequencyCacheImg, m_TotalSizeLog2);
}