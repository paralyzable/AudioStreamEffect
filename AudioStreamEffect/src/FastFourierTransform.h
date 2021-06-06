#pragma once
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

static void FFT(std::vector<float>& real, std::vector<float>& img, const size_t M)
{
    const size_t N = 1ULL << M;
    //decompose signal
    for (size_t i = 1, j = N / 2; i <= N - 2; i++)
    {
        if (i < j)
        {
            std::swap(real[i], real[j]);
            std::swap(img[i], img[j]);
        }
        size_t k = N / 2;
        while(k <= j) 
        {
            j -= k;
            k /= 2;
        }
        j += k;
    }

    for (size_t stage = 1; stage <= M; stage++)
    {
        size_t length = 1ULL << stage;
        float sr = cos(2.0f * (float)M_PI / (float)length);
        float si = sin(2.0f * (float)M_PI / (float)length);
        float ur = 1.0f;
        float ui = 0.0f;
        for (size_t j = 1; j <= length / 2; j++)
        {
            for (size_t i = j - 1; i <= N - 1; i += length)
            {
                size_t ip = i + length / 2;
                float tr = real[ip] * ur - img[ip] * ui;
                float ti = real[ip] * ui + img[ip] * ur;
                real[ip] = real[i] - tr;
                img[ip] = img[i] - ti;
                real[i] += tr;
                img[i] += ti;
            }
            float tr = ur;
            ur = tr * sr - ui * si;
            ui = tr * si + ui * sr;
        }
    }
}

static void IFFT(std::vector<float>& real, std::vector<float>& img, const size_t M)
{
    std::for_each(img.begin(), img.end(), [](float& i) {i = -i; });

    FFT(real, img, M);

    const size_t N = 1ULL << M;
    std::for_each(img.begin(), img.end(), [&](float& i) {i = -i / N; });
    std::for_each(real.begin(), real.end(), [&](float& r) {r = r / N; });
}

static void MulComplexVec(std::vector<float>& r1, std::vector<float>& i1, const std::vector<float>& r2, const std::vector<float>& i2, const size_t M)
{
    const size_t N = 1ULL << M;
    for (size_t i = 0; i < N; i++)
    {
        float r3 = r1[i] * r2[i] - i1[i] * i2[i];
        float i3 = i1[i] * r2[i] + r1[i] * i2[i];

        r1[i] = r3;
        i1[i] = i3;
    }
}