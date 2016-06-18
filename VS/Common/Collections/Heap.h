#pragma once

#include "List.h"

template <typename T>
class Heap
{
    typedef int (*ShouldSwap)(T parent, T child);

public:
    
    List<T> data;
    ShouldSwap compare;

    Heap(ShouldSwap swap)
        : data(), compare(swap)
    { }
    
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
        for (int i = 0; i < data.size; i++)
        {
            if (compare(data[(i-1)/2], data[i]))
                return false;
        }
        return true;
    }
    
    int push(T t)
    {
        int cur = data.size;
        data.add(t);
        
        int done = false;
        while (!done)
        {
            int parent = (cur-1)/2;
            if (parent == cur)
                break;
            if (compare(data[parent], data[cur]))
            {
                T tmp = data[cur];
                data[cur] = data[parent];
                data[parent] = tmp;
                cur = parent;
            }
            else
            {
                done = true;
            }
        }
        
        return true;
    }
    
    T pop()
    {
        T val = data[0];
        data[0] = data[data.size-1];
        data.remove(data.size-1);
        
        int cur = 0;
        bool done = false;
        while (!done)
        {
            int left = cur * 2 + 1;
            int right = cur * 2 + 2;
            int next;
            
            bool leftVsCur = left < data.size && compare(data[cur],data[left]);
            bool rightVsCur = right < data.size && compare(data[cur],data[right]);
            bool leftVsRight = left < data.size && right < data.size && compare(data[left],data[right]);
            
            if (!leftVsCur && !rightVsCur)
            {
                done = true;
                break;
            }
            else if (leftVsCur)
            {
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

        return val;
    }
};