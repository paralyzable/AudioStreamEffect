#pragma once
#include "stream_transform/StreamTransformBase.h"

class PitchStreamTransform :public StreamTransform
{
protected:
	double m_Pitch = 1.0;

public:
	PitchStreamTransform() = default;
	virtual ~PitchStreamTransform() = default;

	PitchStreamTransform(PitchStreamTransform&& other) = default;

	void EndTransform(Track & track, size_t offset, size_t count) override;

private:
	std::string IGetTransformName() const override;
	void ITransformDraw() override;

protected:
	virtual std::string IGetPitchName() const = 0;
	virtual void IPitchDraw() {};
};