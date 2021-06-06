#include "FrequencyDomainBase.h"
#include "FastFourierTransform.h"
#include "imgui.h"

#include <cmath>

FrequencyDomainFilter::FrequencyDomainFilter()
{
	ResizeKernel();
}

SampleView FrequencyDomainFilter::Transform(Track& track, size_t offset, size_t count, unsigned int channel)
{
	std::vector<float>& result = m_Result[channel];
	unsigned int cache_offset = m_CacheOffset;
	if (m_CacheOffset + count > m_StreamKernelSize)
	{
		cache_offset = 0;
		uint64_t position = (uint64_t)std::round(track.GetPosition());
		SampleView view = track.GetSampleView(position, track.GetSampleCount(), channel);

		std::fill(m_SampleCacheImg.begin(), m_SampleCacheImg.end(), 0.0f);
		for (size_t i = 0; i < m_StreamKernelSize; i++)
			m_SampleCacheReal[i] = view[i];
		for (size_t i = m_StreamKernelSize; i < m_TotalSizeUP2; i++)
			m_SampleCacheReal[i] = 0.0f;

		FFT(m_SampleCacheReal, m_SampleCacheImg, m_TotalSizeLog2);
		MulComplexVec(m_SampleCacheReal, m_SampleCacheImg, m_FrequencyCacheReal, m_FrequencyCacheImg, m_TotalSizeLog2);
		IFFT(m_SampleCacheReal, m_SampleCacheImg, m_TotalSizeLog2);

		for (size_t i = 0; i < m_KernelSize; i++)
			m_SampleCacheReal[i] += result[m_StreamKernelSize + i];
		std::copy(m_SampleCacheReal.begin(), m_SampleCacheReal.end(), result.begin());
	}

	return { result.data() + cache_offset , count };
}

void FrequencyDomainFilter::EndTransform(Track& track, size_t offset, size_t count)
{
	track.AdvancePosition((double)count);
	if (m_CacheOffset + count > m_StreamKernelSize)
		m_CacheOffset = 0;
	m_CacheOffset += count;
}

std::string FrequencyDomainFilter::IGetFilterTypeName() const
{
	return "Frequency domain: " + IGetFilterName();
}

static unsigned int NextPower2(unsigned int v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;

	return v;
}

void FrequencyDomainFilter::IFilterTypeDraw(bool kernel_update)
{
	float size = m_KernelSize;
	if (ImGui::SliderFloat("Kernel size", &size, 1, 1000001, "%.0f", ImGuiSliderFlags_Logarithmic))
	{
		m_KernelSize = (int)std::round(size);
		kernel_update = true;
	}
	m_KernelSize += m_KernelSize % 2 == 0;

	float stream_size = m_StreamKernelSize;
	if (ImGui::SliderFloat("Stream kernel size", &stream_size, 256, 1000001, "%.0f", ImGuiSliderFlags_Logarithmic))
	{
		m_StreamKernelSize = (int)std::round(stream_size / 256.0f) * 256;
		kernel_update = true;
	}

	if (kernel_update)
	{
		ResizeKernel();
	}
	IFilterDraw(kernel_update);
}

void FrequencyDomainFilter::ResizeKernel()
{
	m_TotalSizeUP2 = NextPower2(m_KernelSize + m_StreamKernelSize);
	m_SampleCacheReal.resize(m_TotalSizeUP2);
	m_SampleCacheImg.resize(m_TotalSizeUP2);
	m_FrequencyCacheReal.resize(m_TotalSizeUP2);
	m_FrequencyCacheImg.resize(m_TotalSizeUP2);
	m_TotalSizeLog2 = (size_t)std::round(log2(m_TotalSizeUP2));
	for (size_t i = 0; i < m_Result.size(); i++)
		m_Result[i].resize(m_TotalSizeUP2);
}