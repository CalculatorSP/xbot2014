#pragma once

#include "ModBuffer.h"

template <typename T, size_t N>
class FirFilter
{
	ModBuffer<float, N> _kernel;
	ModBuffer<T, N> _data;

public:
	FirFilter(const float *weights)
	{
		for (int i = 0; i < N; ++i)
			_kernel.deposit(weights[i]);
	}

};