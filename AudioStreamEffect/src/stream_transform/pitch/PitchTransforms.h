#pragma once
#include "PitchStreamTransform.h"

//todo
class PhaseVocoderPST :public PitchStreamTransform
{
public:
	PhaseVocoderPST() = default;
	PhaseVocoderPST(PitchStreamTransform&& base);
	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetPitchName() const override { return "Phase Vocoder"; }

	void IPitchDraw() override;
};