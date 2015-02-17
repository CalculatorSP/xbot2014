#pragma once

#include "ModBuffer.h"

template <typename T, int N, typename U=float>
class FirFilter
{
	ModBuffer<U, N> _kernel;
	ModBuffer<T, N> _data;

public:
	FirFilter()
	{ }

	void setKernel(const ModBuffer<U, N> &kernel)
	{
		_kernel = kernel;
	}

	T process(const T &item)
	{
		_data.deposit(item);
		if (_data.isFull())
			return _data.reverseWeightedSum(_kernel);
		else
			return item;
	}

	int getFilterDelay()
	{
		return N / 2;
	}

};