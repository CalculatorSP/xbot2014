#pragma once

template <typename T>
class RunningStats
{
	T _sum;
	size_t _count;

public:
	RunningStats() : _sum(0), _count(0)
	{ }

	void deposit(const T &item)
	{
		_sum += item;
		++_count;
	}

	void remove(const T &item)
	{
		_sum -= item;
		--_count;
	}
	
	size_t getCount()
	{
		return _count;
	}

	T getMean()
	{
		return _sum * (1.0 / _count);
	}
};