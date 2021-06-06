#include "FilterBase.h"
#include "imgui.h"

FilterStreamTransform::FilterStreamTransform(FilterStreamTransform&& other) noexcept
	:StreamTransform(std::move(static_cast<StreamTransform&>(other)))
{
	m_Window = std::move(other.m_Window);
}

void FilterStreamTransform::EndTransform(Track& track, size_t offset, size_t count)
{
	track.AdvancePosition((double)count);
}

std::string FilterStreamTransform::IGetTransformName() const
{
	return std::string("Filter - ") + IGetFilterTypeName();
}

void FilterStreamTransform::ITransformDraw()
{
	int current = m_Window->ID();
	bool window_update(false);
	if (ImGui::Combo("Window", &current, windows_list, std::size(windows_list)))
	{
		window_update = true;
		m_Window = CreateWindowFromID(current);
	}
	window_update |= m_Window->Draw();
	IFilterTypeDraw(window_update);
} 

