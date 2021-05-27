#pragma once
#include "StreamTransform.h"
#include "WindowFunction.h"

class FilterStreamTransform :public StreamTransform
{
protected:
	unsigned int m_KernelSize = 101;
	std::unique_ptr<WindowFunction> m_Window = std::make_unique<RectangleWindow>();
	std::vector<float> m_WindowCache;

public:
	FilterStreamTransform();
	virtual ~FilterStreamTransform() = default;

	FilterStreamTransform(const FilterStreamTransform& other);

	void EndTransform(Track& track, size_t offset, size_t count) override;

private:
	std::string IGetTransformName() const override;
	void ITransformDraw() override;
	void RegenerateCache();

protected:
	virtual std::string IGetFilterName() const = 0;
	virtual void IFilterDraw(bool kernel_update) {};
};

class LowPassFST :public FilterStreamTransform
{
private:
	double m_CutoffFraction = 1.0;
	std::vector<float> m_SincCache;

public:
	LowPassFST();
	LowPassFST(const FilterStreamTransform& base);
	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetFilterName() const override { return "Low pass windowed sinc"; }
	void IFilterDraw(bool kernel_update) override;
	void RegenerateCache();
};

class HighPassFST :public FilterStreamTransform
{
private:
	double m_CutoffFraction = 0.0;
	std::vector<float> m_SincCache;

public:
	HighPassFST();
	HighPassFST(const FilterStreamTransform& base);

	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetFilterName() const override { return "High pass windowed sinc"; }
	void IFilterDraw(bool kernel_update) override;
	void RegenerateCache();
};

class BandPassFST :public FilterStreamTransform
{
private:
	double m_CutoffLow = 0.0;
	double m_CutoffHigh = 1.0;
	std::vector<float> m_SincCache;

public:
	BandPassFST();
	BandPassFST(const FilterStreamTransform& base);

	SampleView Transform(Track& track, size_t offset, size_t count, unsigned int channel) override;

private:
	std::string IGetFilterName() const override { return "Band pass windowed sinc"; }
	void IFilterDraw(bool kernel_update) override;
	void RegenerateCache();
};