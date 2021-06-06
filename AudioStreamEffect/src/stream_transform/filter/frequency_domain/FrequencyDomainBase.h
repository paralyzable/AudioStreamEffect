#pragma once
#include "stream_transform/filter/FilterBase.h"

class FrequencyDomainFilter : public FilterStreamTransform
{
private:
	unsigned int m_TotalSizeUP2 = 512;
	unsigned int m_StreamKernelSize = 256;
	unsigned int m_CacheOffset = 256;

	std::vector<float> m_SampleCacheReal;
	std::vector<float> m_SampleCacheImg;

protected:
	unsigned int m_KernelSize = 101;
	unsigned int m_TotalSizeLog2 = 9;

	std::vector<float> m_FrequencyCacheReal;
	std::vector<float> m_FrequencyCacheImg;

public:
	FrequencyDomainFilter();

	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;
	void EndTransform(Track& track, size_t offset, size_t count) override;

private:
	virtual std::string IGetFilterTypeName() const override;
	virtual void IFilterTypeDraw(bool kernel_update) override;
	void ResizeKernel();

protected:
	virtual std::string IGetFilterName() const = 0;
	virtual bool IFilterDraw(bool kernel_update) = 0;
};