#pragma once

template <typename T>
class RunningStats
{
	T _mean;
	int _count;

public:
	RunningStats()
	{ 
		reset();
	}

	void deposit(const T &item)
	{
		++_count;
		if (_count == 1)
			_mean = item;
		else
		{
			_mean = _mean * (1.0 - (1.0 / _count)) + item * (1.0 / _count);
		}
	}

	void remove(const T &item)
	{
		_mean -= item * (1.0 / _count);
		_mean *= (_count / (_count - 1.0));
		--_count;
	}

	void reset()
	{
		_count = 0;
	}
	
	int getCount()
	{
		return _count;
	}

	T getMean()
	{
		return _mean;
	}
};