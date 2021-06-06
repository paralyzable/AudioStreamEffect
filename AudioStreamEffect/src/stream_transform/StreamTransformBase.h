#pragma once
#include "Track.h"
#include <array>
#include <vector>
#include <string>

class StreamTransform
{
protected:
	std::array<std::vector<float>, 2> m_Result;

public:
	StreamTransform() = default;
	virtual ~StreamTransform() = default;
	
	StreamTransform(StreamTransform&& other) noexcept = default;

	virtual SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) = 0;
	virtual void EndTransform(Track& track, size_t offset, size_t count) = 0;
	bool Draw();

protected:
	virtual std::string IGetTransformName() const = 0;
	virtual void ITransformDraw() = 0;
};