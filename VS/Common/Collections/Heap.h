#pragma once

#include <stdio.h>

template <typename T>
class Heap
{
    typedef int (*ShouldSwap)(T parent, T child);

    public:
    
    int capacity;
    int size;
    T* data;
    ShouldSwap compare;

    Heap(int maxSize, ShouldSwap swap)
    :capacity(maxSize), size(0)
    {
        data = new T[maxSize];
        compare = swap;
    }
    
    ~Heap()
    {
        delete[] data;
    }
    
    T operator[](int i)
    {
        return data[i];
    }
    
    const T& operator[](int i) const
    {
        return data[i];
    }
    
    int isValid()
    {
        for (int i = 0; i < size; i++)
        {
            if (compare(data[(i-1)/2], data[i]))
                return false;
        }
        return true;
    }
    
    int push(T t)
    {
        if (size >= capacity)
            return false;
        
        int cur = size;
        data[size++] = t;
        
        int done = false;
        while (!done)
        {
            if (compare(data[(cur-1)/2], data[cur]))
            {
                T tmp = data[cur];
                data[cur] = data[(cur-1)/2];
                data[(cur-1)/2] = tmp;
                cur = (cur-1)/2;
            }
            else
            {
                done = true;
            }
        }
        
//        if (!isValid())
//            printf("BROKEN\n");
        return true;
    }
    
    T pop()
    {
        if (size == 0)
            return (T)0; //Well this should be fun...
        
        T val = data[0];
        data[0] = data[--size];
        
        int cur = 0;
        bool done = false;
        while (!done)
        {
            int left = cur * 2 + 1;
            int right = cur * 2 + 2;
            int next;
            
            bool leftVsCur = left < size && compare(data[cur],data[left]);
            bool rightVsCur = right < size && compare(data[cur],data[right]);
            bool leftVsRight = left < size && right < size && compare(data[left],data[right]);
            
            if (!leftVsCur && !rightVsCur)
            {
                done = true;
                break;
            }
            else if (leftVsCur)
            {
                //Swap left
                next = left;
                if (leftVsRight)
                    next = right;
            }
            else if (rightVsCur)
            {
                next = right;
            }
            
            T tmp = data[cur];
            data[cur] = data[next];
            data[next] = tmp;
            
            cur = next;
        }
        
//        if (!isValid())
//            printf("BROKEN\n");

        return val;
    }
};