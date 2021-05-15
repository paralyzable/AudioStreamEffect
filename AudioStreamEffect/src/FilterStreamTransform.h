#pragma once
#include "StreamTransform.h"

class FilterStreamTransform :public StreamTransform
{
protected:
	unsigned int m_KernelSize = 100;

public:
	FilterStreamTransform() = default;
	virtual ~FilterStreamTransform() = default;

	void EndTransform(Track& track, size_t offset, size_t count) override;

private:
	std::string IGetTransformName() const override;
	void ITransformDraw() override;

protected:
	virtual std::string IGetFilterName() const = 0;
	virtual void IFilterDraw() {};
};

class LowPassFST :public FilterStreamTransform
{
private:
	double m_CutoffFraction = 1.0;

public:
	LowPassFST() = default;
	LowPassFST(const FilterStreamTransform& base);
	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetFilterName() const override { return "Low pass windowed sinc"; }
	void IFilterDraw() override;
};

class HighPassFST :public FilterStreamTransform
{
private:
	double m_CutoffFraction = 0.0;

public:
	HighPassFST() = default;
	HighPassFST(const FilterStreamTransform& base);

	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetFilterName() const override { return "High pass windowed sinc"; }
	void IFilterDraw() override;
};

class BandPassFST :public FilterStreamTransform
{
private:
	double m_CutoffLow = 0.0;
	double m_CutoffHigh = 1.0;

public:
	BandPassFST() = default;
	BandPassFST(const FilterStreamTransform& base);

	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetFilterName() const override { return "Band pass windowed sinc"; }
	void IFilterDraw() override;
};