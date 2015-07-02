#pragma once

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
    int _count;

public:
    ModBuffer() 
        : _head(0), _count(0)
    { }

    ModBuffer(const T init[N])
        : _head(0), _count(0)
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

        if (++_count > N)
            _count = N;

        if (_head >= N)
            _head = 0;
    }

    void reset()
    {
        _head = 0;
        _count = 0;
    }

    int getCount() const
    {
        return _count;
    }

    bool isFull() const
    {
        return _count == N;
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