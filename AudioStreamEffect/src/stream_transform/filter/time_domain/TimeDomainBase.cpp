#include "TimeDomainBase.h"
#include "imgui.h"

#include <cmath>

SampleView TimeDomainFilter::Transform(Track& track, size_t offset, size_t count, unsigned int channel)
{
	SampleView view = track.GetSampleView(0, track.GetSampleCount(), channel);
	uint64_t position = (uint64_t)std::round(track.GetPosition());

	m_Result[channel].resize(count);
	for (size_t i = 0; i < count; i++)
	{
		int L = ((int)m_KernelSize - 1) / 2;
		float result(0.0);
		for (int n = 0; n < m_KernelSize; n++)
			result += view[(size_t)(position + n - L)] * m_KernelCache[n];
		m_Result[channel][i] = result;
		position += 1.0;
	}

	return { m_Result[channel].data(), count };
}

std::string TimeDomainFilter::IGetFilterTypeName() const
{
	return "Time domain: " + IGetFilterName();
}

void TimeDomainFilter::IFilterTypeDraw(bool kernel_update)
{
	float size = m_KernelSize;
	if (ImGui::SliderFloat("Kernel size", &size, 1, 20001, "%.0f", ImGuiSliderFlags_Logarithmic))
	{
		m_KernelSize = (int)std::round(size);
		kernel_update = true;
	}
	m_KernelSize += m_KernelSize % 2 == 0;

	if (IFilterDraw(kernel_update))
	{
		int L = ((int)m_KernelSize - 1) / 2;
		for (int x = -L; x <= L; x++)
			m_KernelCache[x + L] *= (*m_Window)(x, m_KernelSize);
	}
}