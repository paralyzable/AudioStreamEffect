#pragma once
#include "stream_transform/StreamTransformBase.h"
#include "WindowFunction.h"

class FilterStreamTransform :public StreamTransform
{
protected:
	std::unique_ptr<WindowFunction> m_Window = std::make_unique<RectangleWindow>();

public:
	FilterStreamTransform() = default;
	virtual ~FilterStreamTransform() = default;
	FilterStreamTransform(FilterStreamTransform&& other) noexcept;

	virtual void EndTransform(Track& track, size_t offset, size_t count) override;

private:
	std::string IGetTransformName() const override;
	void ITransformDraw() override;

protected:
	virtual std::string IGetFilterTypeName() const = 0;
	virtual void IFilterTypeDraw(bool kernel_update)  = 0;
};