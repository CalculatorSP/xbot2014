#pragma once

#include "ModBuffer.h"

template <typename T, int P, int Q, typename U=float>
class IirFilter
{
	ModBuffer<U, Q> _a;
	ModBuffer<U, P> _b;
	ModBuffer<T, P> _x;
	ModBuffer<T, Q> _y;

	T _compute() const
	{
		if (!_x.isFull() || !_y.isFull())
			return (T)0;

		return _x.reverseWeightedSum(_b) - _y.reverseWeightedSum(_a);
	}

public:
	IirFilter()
	{ }

	void setA(const ModBuffer<U, Q> &a)
	{
		_a = a;
	}

	void setB(const ModBuffer<U, P> &b)
	{
		_b = b;
	}

	T process(const T &item)
	{
		_x.deposit(item);
		T yn = _compute();
		_y.deposit(yn);
		return yn;
	}
};

typedef ModBuffer<float, 1> ModBuf1f;
typedef ModBuffer<float, 2> ModBuf2f;
typedef ModBuffer<float, 3> ModBuf3f;

typedef IirFilter<float, 1, 1> Iir1f;
typedef IirFilter<float, 2, 2> Iir2f;
typedef IirFilter<float, 3, 2> Biquadf;
