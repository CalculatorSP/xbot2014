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

    void deposit(const T &item)
    {
        _data.deposit(item);
    }

    T compute() const
    {
        if (_data.isFull())
            return _data.reverseWeightedSum(_kernel);
        else
            return (T)0;
    }

    T compute2() const
    {
        if (_data.isFull())
            return _data.weightedSum(_kernel);
        else
            return (T)0;
    }

    T process(const T &item)
    {
        _data.deposit(item);
        if (_data.isFull())
            return _data.reverseWeightedSum(_kernel);
        else
            return item;
    }

    T process2(const T &item)
    {
        _data.deposit(item);
        if (_data.isFull())
            return _data.weightedSum(_kernel);
        else
            return item;
    }

    int getFilterDelay() const
    {
        return N / 2;
    }

};
