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
		int start = (int)std::round(position - m_KernelSize / 2);
		int end = (int)std::round(position + m_KernelSize / 2);
		float result(0.0);
		for (int j = start; j < end; j++)
		{
			float x = (float)(M_PI * (j - position));
			float blackman_window = (0.42f + 0.5f * cos(2.0f * x / m_KernelSize) + 0.08f * cos(4.0f * x / m_KernelSize));
			float zero = x == 0;
			float sinc = sin(x) / (x + zero) * (1.0f - zero) + zero;
			result += view[(size_t)j] * sinc * blackman_window;
		}
		m_Result[channel][i] = result;
		position += m_SpeedMultiple;
	}

	return { m_Result[channel].data(), count };
}

void WindowedSincSST::ISpeedDraw()
{
	unsigned int umin = 0, umax = 500;
	ImGui::SliderScalar("Kernel size", ImGuiDataType_U32, &m_KernelSize, &umin, &umax);
	m_KernelSize += m_KernelSize % 2 == 1;
}
