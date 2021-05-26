#pragma once
#include "StreamTransform.h"
#include "WindowFunction.h"
#include <memory>

class SpeedStreamTransform :public StreamTransform
{
protected:
	double m_SpeedMultiple = 1.0;

public:
	SpeedStreamTransform() = default;
	virtual ~SpeedStreamTransform() = default;

	void EndTransform(Track& track, size_t offset, size_t count) override;

private:
	std::string IGetTransformName() const override;
	void ITransformDraw() override;

protected:
	virtual std::string IGetSpeedName() const = 0;
	virtual void ISpeedDraw() {};
};

class NearestNeighbourSST : public SpeedStreamTransform
{
public:
	NearestNeighbourSST() = default;
	NearestNeighbourSST(const SpeedStreamTransform& base);
	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetSpeedName() const override { return "Nearest neighbour"; }
};

class LinearSST :public SpeedStreamTransform
{
public:
	LinearSST() = default;
	LinearSST(const SpeedStreamTransform& base);
	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetSpeedName() const override { return "Linear"; }
};

class CubicSST : public SpeedStreamTransform
{
public:
	CubicSST() = default;
	CubicSST(const SpeedStreamTransform& base);
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
	WindowedSincSST(const SpeedStreamTransform& base);
	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetSpeedName() const override { return "Windowed sinc"; }

	void ISpeedDraw() override;
};