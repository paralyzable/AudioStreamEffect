#include "PitchBase.h"
#include "imgui.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

void PitchStreamTransform::EndTransform(Track& track, size_t offset, size_t count)
{
	track.AdvancePosition((double)count);
}

std::string PitchStreamTransform::IGetTransformName() const
{
	return "Pitch - " + IGetPitchName();
}

void PitchStreamTransform::ITransformDraw()
{
	double min = 0.1, max = 2.0;
	ImGui::SliderScalar("Pitch", ImGuiDataType_Double, &m_Pitch, &min, &max);
	IPitchDraw();
}
