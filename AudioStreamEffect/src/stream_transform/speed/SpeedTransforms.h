#pragma once
#include "SpeedBase.h"

#include <memory>

class NearestNeighbourSST : public SpeedStreamTransform
{
public:
	NearestNeighbourSST() = default;
	NearestNeighbourSST(SpeedStreamTransform&& base);
	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetSpeedName() const override { return "Nearest neighbour"; }
};

class LinearSST :public SpeedStreamTransform
{
public:
	LinearSST() = default;
	LinearSST(SpeedStreamTransform&& base);
	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetSpeedName() const override { return "Linear"; }
};

class CubicSST : public SpeedStreamTransform
{
public:
	CubicSST() = default;
	CubicSST(SpeedStreamTransform&& base);
	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetSpeedName() const override { return "Cubic"; }
};

class WindowedSincSST :public SpeedStreamTransform
{
private:
	unsigned int m_KernelSize = 50;
	std::unique_ptr<WindowFunction> m_Window = std::make_unique<RectangleWindow>();

public:
	WindowedSincSST() = default;
	WindowedSincSST(SpeedStreamTransform&& base);
	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetSpeedName() const override { return "Windowed sinc"; }

	void ISpeedDraw() override;
};