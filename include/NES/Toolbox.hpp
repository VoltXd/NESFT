#pragma once

#include <string>
#include <array>
#include <complex>
#include <deque>
#include "NES/Config.hpp"

void testAndExitWithMessage(bool condition, const std::string& message);
void traceLog(const std::string& log);

template <typename T>
void popAndPush(std::deque<T>& fifo, T value)
{
	fifo.pop_front();
	fifo.push_back(value);
}

template <typename T, size_t S>
s32 getScopeTriggerOffset(const std::array<T, S>& array)
{
    T minValue = *std::min_element(array.begin(), array.end());
    T maxValue = *std::max_element(array.begin(), array.end());
    T average = 0.5f * (minValue + maxValue);

    s32 sampleMiddleOffset = 0;
    for (int i = (BUFFER_SIZE / 4); i < (3 * (BUFFER_SIZE / 4)); i++)
    {
        T currentValue = array[i];
        T nextValue = array[i + 1];
        if (currentValue <= average && average < nextValue)
        {
            sampleMiddleOffset = i;
            break;
        }
    }

    return sampleMiddleOffset > 0 ? sampleMiddleOffset - (BUFFER_SIZE / 4) : 0;
}

template <unsigned int S>
void fftMagnitude(const std::array<float, S>& signal, std::array<float, S>& spectrum)
{
	using complex = std::complex<float>;
	
    // Copy signal to complex array
	std::array<complex, S> x;
	for (u32 i = 0; i < S; i++)
        x[i] = signal[i];

	// DFT
	unsigned int N = S, k = N, n;
	float thetaT = 3.14159265358979323846264338328f / N;
	complex phiT = complex(cosf(thetaT), -sinf(thetaT)), T;
	while (k > 1)
	{
		n = k;
		k >>= 1;
		phiT = phiT * phiT;
		T = 1.0L;
		for (unsigned int l = 0; l < k; l++)
		{
			for (unsigned int a = l; a < N; a += n)
			{
				unsigned int b = a + k;
				complex t = x[a] - x[b];
				x[a] += x[b];
				x[b] = t * T;
			}
			T *= phiT;
		}
	}
	// Decimate
	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++)
	{
		unsigned int b = a;
		// Reverse bits
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a)
		{
			complex t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}

    // Retrieve magnitude
    for (u32 i = 0; i < S; i++)
        spectrum[i] = std::abs(x[i]) * (1.0f / S);
}

extern bool gIsTraceLogCpuEnabled;
extern bool gIsTraceLogPpuEnabled;
extern bool gIsTraceLogMMC3IrqEnabled;
