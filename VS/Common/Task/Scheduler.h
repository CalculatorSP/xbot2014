#pragma once

#include "Collections/Heap.h"
#include "Job.h"

#include <time.h>
#include <Windows.h>

int shouldSwap(IJob* parent, IJob* child)
{
    return parent->time >= child->time;
}

class Scheduler
{
public:
    Heap<IJob*> jobs;
    
    Scheduler()
        :jobs(shouldSwap)
    {
        
    }
    
    long getTime()
    {
        struct timeval tp;
        gettimeofday(&tp);
        long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
        return ms;
    }
    
    void post(Job::Runnable d)
    {
        long ms = getTime();
        Job* j = new Job(ms,d);
        jobs.push(j);
    }
    
    template <typename T>
    void post(typename Job2<T>::Delegate d, T arg)
    {
        long ms = getTime();
        Job2<T>* j = new Job2<T>(ms, d, arg);
        jobs.push(j);
    }
    
    template <typename CLAZZ, typename T>
    void post(typename Job3<CLAZZ, T>::Delegate d, CLAZZ* c, T arg)
    {
        long ms = getTime();
        Job3<CLAZZ, T>* j = new Job3<CLAZZ, T>(ms, d, c, arg);
        jobs.push(j);
    }
    
    template <typename CLAZZ, typename T>
    void post(typename Job4<CLAZZ>::Delegate d, CLAZZ* c)
    {
        long ms = getTime();
        Job4<CLAZZ>* j = new Job4<CLAZZ>(ms, d, c);
        jobs.push(j);
    }
    
    void postDelayed(long millisDelay, Job::Runnable d)
    {
        long ms = getTime() + millisDelay;
        Job* j = new Job(ms, d);
        jobs.push(j);
    }
    
    template <typename T>
    void postDelayed(long millisDelay, typename Job2<T>::Delegate d, T arg)
    {
        long ms = getTime() + millisDelay;
        Job2<T>* j = new Job2<T>(ms, d, arg);
        jobs.push(j);
    }
    
    template <typename CLAZZ, typename T>
    void postDelayed(long millisDelay, typename Job3<CLAZZ, T>::Delegate d, CLAZZ* c, T arg)
    {
        long ms = getTime() + millisDelay;
        Job3<CLAZZ, T>* j = new Job3<CLAZZ, T>(ms, d, c, arg);
        jobs.push(j);
    }
    
    template <typename CLAZZ, typename T>
    void postDelayed(long millisDelay, typename Job4<CLAZZ>::Delegate d, CLAZZ* c)
    {
        long ms = getTime() + millisDelay;
        Job4<CLAZZ>* j = new Job4<CLAZZ>(ms, d, c);
        jobs.push(j);
    }
    
    void postAtTime(long time, Job::Runnable d)
    {
        long ms = time;
        Job* j = new Job(time, d);
        jobs.push(j);
    }
    
    template <typename T>
    void postAtTime(long time, typename Job2<T>::Delegate d, T arg)
    {
        long ms = time;
        Job2<T>* j = new Job2<T>(ms, d, arg);
        jobs.push(j);
    }
    
    template <typename CLAZZ, typename T>
    void postAtTime(long time, typename Job3<CLAZZ, T>::Delegate d, CLAZZ* c, T arg)
    {
        long ms = time;
        Job3<CLAZZ, T>* j = new Job3<CLAZZ, T>(ms, d, c, arg);
        jobs.push(j);
    }
    
    template <typename CLAZZ, typename T>
    void postAtTime(long time, typename Job4<CLAZZ>::Delegate d, CLAZZ* c)
    {
        long ms = time;
        Job4<CLAZZ>* j = new Job4<CLAZZ>(ms, d, c);
        jobs.push(j);
    }
    
    bool run()
    {
        if (jobs.data.size == 0)
            return false;
        IJob* j = jobs.pop();
        long time = j->time - getTime();
        if (time >= 0)
            Sleep(time);

        j->operator()();
        delete j;
        return true;
    }

	static int gettimeofday(struct timeval *tv)
	{
		FILETIME ft;
		unsigned __int64 tmpres = 0;

		if (tv != NULL)
		{
			GetSystemTimeAsFileTime(&ft);

			tmpres |= ft.dwHighDateTime;
			tmpres <<= 32;
			tmpres |= ft.dwLowDateTime;

			tmpres /= 10;

			tv->tv_sec = (long)(tmpres / 1000000UL);
			tv->tv_usec = (long)(tmpres % 1000000UL);
		}

		return 0;
	}
};
