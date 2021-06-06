#pragma once
#include "TimeDomainBase.h"

class LowPassTF :public TimeDomainFilter
{
private:
	double m_CutoffFraction = 1.0;

public:
	LowPassTF();
	LowPassTF(TimeDomainFilter&& base);

private:
	std::string IGetFilterName() const override;
	bool IFilterDraw(bool kernel_update) override;
	void RegenerateCache();
};

class HighPassTF :public TimeDomainFilter
{
private:
	double m_CutoffFraction = 0.0;

public:
	HighPassTF();
	HighPassTF(TimeDomainFilter&& base);

private:
	std::string IGetFilterName() const override;
	bool IFilterDraw(bool kernel_update) override;
	void RegenerateCache();
};

class BandPassTF :public TimeDomainFilter
{
private:
	double m_CutoffLow = 0.0;
	double m_CutoffHigh = 1.0;

public:
	BandPassTF();
	BandPassTF(TimeDomainFilter&& base);

private:
	std::string IGetFilterName() const override;
	bool IFilterDraw(bool kernel_update) override;
	void RegenerateCache();
};

class BandRejectTF :public TimeDomainFilter
{
private:
	double m_CutoffLow = 0.1;
	double m_CutoffHigh = 0.1;

public:
	BandRejectTF();
	BandRejectTF(TimeDomainFilter&& base);

private:
	std::string IGetFilterName() const override;
	bool IFilterDraw(bool kernel_update) override;
	void RegenerateCache();
};