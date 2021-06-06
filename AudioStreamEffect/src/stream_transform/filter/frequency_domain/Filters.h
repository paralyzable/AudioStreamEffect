#pragma once
#include "FrequencyDomainBase.h"

class LowPassFF :public FrequencyDomainFilter
{
private:
	double m_CutoffFraction = 1.0;

public:
	LowPassFF();
	LowPassFF(FrequencyDomainFilter&& base);

private:
	std::string IGetFilterName() const override;
	bool IFilterDraw(bool kernel_update) override;
	void RegenerateCache();
};

class HighPassFF :public FrequencyDomainFilter
{
private:
	double m_CutoffFraction = 0.0;

public:
	HighPassFF();
	HighPassFF(FrequencyDomainFilter&& base);

private:
	std::string IGetFilterName() const override;
	bool IFilterDraw(bool kernel_update) override;
	void RegenerateCache();
};

class BandPassFF :public FrequencyDomainFilter
{
private:
	double m_CutoffLow = 0.0;
	double m_CutoffHigh = 1.0;

public:
	BandPassFF();
	BandPassFF(FrequencyDomainFilter&& base);

private:
	std::string IGetFilterName() const override;
	bool IFilterDraw(bool kernel_update) override;
	void RegenerateCache();
};

class BandRejectFF :public FrequencyDomainFilter
{
private:
	double m_CutoffLow = 0.1;
	double m_CutoffHigh = 0.1;

public:
	BandRejectFF();
	BandRejectFF(FrequencyDomainFilter&& base);

private:
	std::string IGetFilterName() const override;
	bool IFilterDraw(bool kernel_update) override;
	void RegenerateCache();
};