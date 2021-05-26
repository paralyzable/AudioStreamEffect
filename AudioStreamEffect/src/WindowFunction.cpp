#include "WindowFunction.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include "imgui.h"

std::unique_ptr<WindowFunction> CreateWindowFromID(int id)
{
    switch (id)
    {
    default:
    case 0:
        return std::make_unique<RectangleWindow>();
    case 1:
        return std::make_unique<TriangleWindow>();
    case 2:
        return std::make_unique<ParzenWindow>();
    case 3:
        return std::make_unique<HannWindow>();
    case 4:
        return std::make_unique<BlackmanWindow>();
    case 5:
        return std::make_unique<DolphChebyshevWindow>();
    }
}

void WindowFunction::Draw()
{
}

float RectangleWindow::operator()(float x, float window_size) const
{
	return 1.0f;
}

float TriangleWindow::operator()(float x, float window_size) const
{
	return 1.0f - abs((2.0f * x) / (window_size + 1.0f));
}

float ParzenWindow::operator()(float x, float window_size) const
{
	float absx = abs(x);
	float L = (window_size + 1.0f) * 0.5f;
	float a = (1.0f - absx / L);
	if (absx <= L * 0.5f)
		return 1.0f - 6.0f * (x / L) * (x / L) * a;
	else
		return 2.0f * a * a * a;
}

float HannWindow::operator()(float x, float window_size) const
{
	float a = cos(M_PI * x / (window_size + 1.0f));
	return a * a;
}

float BlackmanWindow::operator()(float x, float window_size) const
{
	float a = M_PI * x / (window_size + 1.0f);
	return 0.42f + 0.5f * cos(2.0f * a) + 0.08f * cos(4.0f * a);
}

float DolphChebyshevWindow::operator()(float x, float window_size) const
{//heavily botched version from http://practicalcryptography.com/miscellaneous/machine-learning/implementing-dolph-chebyshev-window/
    const auto& cheby_poly = [](int n, double x)->double
    {
        if (abs(x) <= 1)
            return cos(n * acos(x));
        else
            return cosh(n * acosh(x));
    };

    int L = (int)(window_size - 1);
    double tg = pow(10.0, m_Attenuation / 20.0);
    double x0 = cosh((1.0 / L) * acosh(tg));
    double sum = 0;
    for (int i = 1; i <= L / 2; i++)
        sum += cheby_poly(L, x0 * cos(M_PI * i / window_size)) * cos(2.0 * x * M_PI * i / window_size);
    return (tg + 2 * sum) / (tg * 3.0);
}

void DolphChebyshevWindow::Draw()
{
    ImGui::SliderFloat("Attenuation(db)", &m_Attenuation, 0.0f, 180.0f);
}