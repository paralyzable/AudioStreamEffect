#pragma once
#include <memory>

const char* const windows_list[] = { "Rectangle", "Triangle", "Parzen", "Hann", "Blackman", "DolphChebyshev" };
class WindowFunction;
std::unique_ptr<WindowFunction> CreateWindowFromID(int id);

// N = window_size(will be odd)
// all window functions have a domain of { -(N+1)/2 <= x <= (N+1)/2 }
// which approaches 0 at the domain edges
class WindowFunction
{
public:
	virtual float operator()(float x, float window_size) const = 0;
	virtual bool Draw();
	virtual int ID() const = 0;
};

class RectangleWindow :public WindowFunction
{
public:
	float operator()(float x, float window_size) const override;
	int ID() const { return 0; }
};

class TriangleWindow :public WindowFunction
{
public:
	float operator()(float x, float window_size) const override;
	int ID() const { return 1; }
};

class ParzenWindow :public WindowFunction
{
public:
	float operator()(float x, float window_size) const override;
	int ID() const { return 2; }
};

class HannWindow :public WindowFunction
{
public:
	float operator()(float x, float window_size) const override;
	int ID() const { return 3; }
};

class BlackmanWindow :public WindowFunction
{
public:
	float operator()(float x, float window_size) const override;
	int ID() const { return 4; }
};

class DolphChebyshevWindow :public WindowFunction
{
private:
	float m_Attenuation = 120;
public:
	float operator()(float x, float window_size) const override;
	bool Draw() override;
	int ID() const { return 5; }
};