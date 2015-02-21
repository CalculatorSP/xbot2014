#pragma once

#include "ModBuffer.h"
#include "RunningStats.h"

template <typename T, int N>
class WindowedStats
{
	ModBuffer<T, N> _data;
	RunningStats<T> _stats;

public:
	WindowedStats()
	{ }

	void reset()
	{
		_data.reset();
		_stats.reset();
	}

	void deposit(const T &item)
	{
		if (_data.isFull())
			_stats.remove(_data[0]);

		_stats.deposit(item);
		_data.deposit(item);
	}

	bool isFull() const
	{
		return _data.isFull();
	}

	int getCount() const
	{
		return _stats.getCount();
	}

	T getMean() const
	{
		return _stats.getMean();
	}
};