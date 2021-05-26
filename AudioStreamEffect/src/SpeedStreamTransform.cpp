#include "SpeedStreamTransform.h"
#include "imgui.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <algorithm>

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

NearestNeighbourSST::NearestNeighbourSST(const SpeedStreamTransform& base)
	:SpeedStreamTransform(base)
{
}

SampleView NearestNeighbourSST::Transform(Track& track, size_t offset, size_t count, unsigned int channel)
{
	SampleView view = track.GetSampleView(0, track.GetSampleCount(), channel);
	double position = track.GetPosition();

	m_Result[channel].resize(count);

	for (size_t i = 0; i < count; i++)
	{
		size_t index = (size_t)std::round(position);
		m_Result[channel][i] = view[index];
		position += m_SpeedMultiple;
	}

	return { m_Result[channel].data(), count };
}

LinearSST::LinearSST(const SpeedStreamTransform& base)
	:SpeedStreamTransform(base)
{
}

SampleView LinearSST::Transform(Track& track, size_t offset, size_t count, unsigned int channel)
{
	SampleView view = track.GetSampleView(0, track.GetSampleCount(), channel);
	double position = track.GetPosition();

	m_Result[channel].resize(count);
	for (size_t i = 0; i < count; i++)
	{
		double start;
		double interp = std::modf(position, &start);
		m_Result[channel][i] = (float)(view[(size_t)start] * (1.0 - interp) + view[(size_t)start + 1] * interp);
		position += m_SpeedMultiple;
	}

	return { m_Result[channel].data(), count };
}

float CubicInterp(float v0, float v1, float v2, float v3, float x)
{
	return v1 + 0.5f * x * (v2 - v0 + x * (2.0f * v0 - 5.0f * v1 + 4.0f * v2 - v3 + x * (3.0f * (v1 - v2) + v3 - v0)));
}

CubicSST::CubicSST(const SpeedStreamTransform& base)
	:SpeedStreamTransform(base)
{
}

SampleView CubicSST::Transform(Track& track, size_t offset, size_t count, unsigned int channel)
{
	SampleView view = track.GetSampleView(0, track.GetSampleCount(), channel);
	double position = track.GetPosition();

	m_Result[channel].resize(count);
	for (size_t i = 0; i < count; i++)
	{
		double start;
		double interp = std::modf(position, &start);
		size_t p = (size_t)start;
		m_Result[channel][i] = CubicInterp(view[p - 1], view[p], view[p + 1], view[p + 2], (float)interp);
		position += m_SpeedMultiple;
	}

	return { m_Result[channel].data(), count };
}

WindowedSincSST::WindowedSincSST(const SpeedStreamTransform& base)
	:SpeedStreamTransform(base)
{
}

SampleView WindowedSincSST::Transform(Track& track, size_t offset, size_t count, unsigned int channel)
{
	SampleView view = track.GetSampleView(0, track.GetSampleCount(), channel);
	double position = track.GetPosition();

	m_Result[channel].resize(count);
	for (size_t i = 0; i < count; i++)
	{
		int start = (int)std::round(position - m_KernelSize / 2.0);
		int end = (int)std::round(position + m_KernelSize / 2.0);
		float result(0.0);
		float angle = (float)sin(M_PI * (start - position));
		for (int j = start; j < end; j++)
		{
			float x = (float)(j - position);
			float window = (*m_Window)(x, m_KernelSize);
			x *= M_PI;
			float zero = x == 0;
			float sinc = angle / (x + zero) * (1.0f - zero) + zero;
			angle = -angle;
			result += view[(size_t)j] * sinc * window;
		}
		m_Result[channel][i] = result;
		position += m_SpeedMultiple;
	}

	return { m_Result[channel].data(), count };
}

void WindowedSincSST::ISpeedDraw()
{
	float size = m_KernelSize;
	if (ImGui::SliderFloat("Kernel size", &size, 1, 10001,"%.0f", 2.0f))
		m_KernelSize = (int)std::round(size);
	m_KernelSize += m_KernelSize % 2 == 0;

	int current = m_Window->ID();
	if (ImGui::Combo("Window", &current, windows_list, std::size(windows_list)))
	{
		m_Window = CreateWindowFromID(current);
		if (current == 5)//DolpCheby is expensive
			m_KernelSize = 15;
	}
	m_Window->Draw();
}
