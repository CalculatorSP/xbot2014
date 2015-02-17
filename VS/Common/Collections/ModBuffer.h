#pragma once

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) < (b)) ? (b) : (a))

template <typename T>
T ModPositive(T x, T n)
{
	T m = x % n;
	while (m < (T)0)
		m += n;

	return m;
}

template <typename T, int N>
class ModBuffer
{
	T _data[N];
	int _head;
	bool _full;

public:
	ModBuffer() 
		: _head(0), _full(false)
	{ }

	ModBuffer(const T init[N])
		: _head(0), _full(false)
	{
		for (int i = 0; i < N; ++i)
			deposit(init[i]);
	}

	T &operator[](int i)
	{
		return _data[ModPositive(_head + i, N)];
	}

	const T &operator[](int i) const
	{
		return _data[ModPositive(_head + i, N)];
	}

	void deposit(const T &item)
	{
		_data[_head++] = item;
		if (_head >= N)
		{
			_head = 0;
			_full = true;
		}
	}

	bool isFull()
	{
		return _full;
	}

	template <typename U> T weightedSum(const ModBuffer<U, N> &weights) const
	{
		T sum = (*this)[0] * (U)0;
		for (int i = 0; i < N; ++i)
			sum += (*this)[i] * weights[i];

		return sum;
	}

	template <typename U> T reverseWeightedSum(const ModBuffer<U, N> &weights) const
	{
		T sum = (*this)[0] * (U)0;
		for (int i = 0; i < N; ++i)
			sum += (*this)[i] * weights[-(i + 1)];

		return sum;
	}

	template <typename U, int M> static void convolve(ModBuffer<T, M + N - 1> &dst, const ModBuffer<T, N> &src, const ModBuffer<U, M> &weights)
	{
		for (int n = 0; n < M + N - 1; ++n)
		{
			T sum = src[n] * (U)0;
			for (int i = MAX(n - N + 1, 0); i < MIN(n + 1, M); ++i)
				sum += src[n - i] * weights[i];

			dst.deposit(sum);
		}
	}

};