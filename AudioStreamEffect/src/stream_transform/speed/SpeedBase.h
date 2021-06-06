#pragma once
#include "stream_transform/StreamTransformBase.h"
#include "WindowFunction.h"

class SpeedStreamTransform :public StreamTransform
{
protected:
	double m_SpeedMultiple = 1.0;

public:
	SpeedStreamTransform() = default;
	virtual ~SpeedStreamTransform() = default;

	SpeedStreamTransform(SpeedStreamTransform&& other) = default;

	void EndTransform(Track& track, size_t offset, size_t count) override;

private:
	std::string IGetTransformName() const override;
	void ITransformDraw() override;

protected:
	virtual std::string IGetSpeedName() const = 0;
	virtual void ISpeedDraw() {};
};