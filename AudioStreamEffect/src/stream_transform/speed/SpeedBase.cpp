#include "SpeedBase.h"
#include "imgui.h"

void SpeedStreamTransform::EndTransform(Track& track, size_t offset, size_t count)
{
	track.AdvancePosition(m_SpeedMultiple * count);
}

std::string SpeedStreamTransform::IGetTransformName() const
{
	return std::string("Speed - ") + IGetSpeedName();
}

void SpeedStreamTransform::ITransformDraw()
{
	double min = 0.1, max = 2.0;
	ImGui::SliderScalar("Speed", ImGuiDataType_Double, &m_SpeedMultiple, &min, &max);
	ISpeedDraw();
}
