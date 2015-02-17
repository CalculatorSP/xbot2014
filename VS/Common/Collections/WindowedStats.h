#pragma once

#include "ModBuffer.h"
#include "RunningStats.h"

template <typename T, size_t N>
class WindowedStats
{
	ModBuffer<T, N> _data;
	RunningStats<T> _stats;

public:
	WindowedStats()
	{ }

	void deposit(const T &item)
	{
		if (_data.isFull())
			_stats.remove(_data[0]);

		_stats.deposit(item);
		_data.deposit(item);
	}

	size_t getCount()
	{
		return _stats.getCount();
	}

	T getMean()
	{
		return _stats.getMean();
	}
};