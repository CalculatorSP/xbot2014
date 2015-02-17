#pragma once

template <typename T>
T ModPositive(T x, T n)
{
	T m = x % n;
	while (m < (T)0)
		m += n;

	return m;
}

template <typename T, size_t N>
class ModBuffer
{
	T _data[N];
	size_t _head;
	bool _full;

public:
	ModBuffer() : _head(0), _full(false)
	{ }

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

	T &operator[](size_t i)
	{
		return _data[ModPositive(_head + i, N)];
	}

	const T &operator[](size_t i) const
	{
		return _data[ModPositive(_head + i, N)];
	}
};