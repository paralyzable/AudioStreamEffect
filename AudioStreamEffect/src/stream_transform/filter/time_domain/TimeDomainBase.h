#pragma once
#include "stream_transform/filter/FilterBase.h"

class TimeDomainFilter : public FilterStreamTransform
{
protected:
	unsigned int m_KernelSize = 101;
	std::vector<float> m_KernelCache;

public:
	TimeDomainFilter() = default;

	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	virtual std::string IGetFilterTypeName() const override;
	virtual void IFilterTypeDraw(bool kernel_update) override;

protected:
	virtual std::string IGetFilterName() const = 0;
	virtual bool IFilterDraw(bool kernel_update) = 0;
};