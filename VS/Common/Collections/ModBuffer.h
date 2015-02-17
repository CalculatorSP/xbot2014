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
	T _elements[N];
	size_t _head;
	bool _full;

public:
	ModBuffer<T, N>() : _head(0), _full(false)
	{ }

	void deposit(const T &item)
	{
		_elements[_head++] = item;
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
		return _elements[ModPositive(i, N)];
	}

	const T &operator[](size_t i) const
	{
		return _elements[ModPositive(i, N)];
	}
};