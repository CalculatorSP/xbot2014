#pragma once

template <typename T>
class RunningStats
{
	T _sum;
	size_t _count;

public:
	RunningStats()
	{ 
		reset();
	}

	void deposit(const T &item)
	{
		if (_count == 0)
			_sum = item;
		else
			_sum += item;

		++_count;
	}

	void remove(const T &item)
	{
		_sum -= item;
		--_count;
	}

	void reset()
	{
		_count = 0;
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