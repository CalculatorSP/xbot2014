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
    
    uint64_t getTime()
    {
        struct timeval tp;
        gettimeofday(&tp);
        uint64_t us = (uint64_t)(tp.tv_sec) * 1000000 + (uint64_t)tp.tv_usec; //get current timestamp in microseconds
        return us;
    }
    
    void post(Job::Runnable d)
    {
        uint64_t us = getTime();
        Job* j = new Job(us,d);
        jobs.push(j);
    }
    
    template <typename T>
    void post(typename Job2<T>::Delegate d, T arg)
    {
        uint64_t us = getTime();
        Job2<T>* j = new Job2<T>(us, d, arg);
        jobs.push(j);
    }
    
    template <typename CLAZZ, typename T>
    void post(typename Job3<CLAZZ, T>::Delegate d, CLAZZ* c, T arg)
    {
        uint64_t us = getTime();
        Job3<CLAZZ, T>* j = new Job3<CLAZZ, T>(us, d, c, arg);
        jobs.push(j);
    }
    
    template <typename CLAZZ, typename T>
    void post(typename Job4<CLAZZ>::Delegate d, CLAZZ* c)
    {
        uint64_t us = getTime();
        Job4<CLAZZ>* j = new Job4<CLAZZ>(us, d, c);
        jobs.push(j);
    }
    
    void postDelayed(uint64_t microsDelay, Job::Runnable d)
    {
        uint64_t us = getTime() + microsDelay;
        Job* j = new Job(us, d);
        jobs.push(j);
    }
    
    template <typename T>
    void postDelayed(uint64_t microsDelay, typename Job2<T>::Delegate d, T arg)
    {
		uint64_t us = getTime() + microsDelay;
		Job2<T>* j = new Job2<T>(us, d, arg);
        jobs.push(j);
    }
    
    template <typename CLAZZ, typename T>
    void postDelayed(uint64_t microsDelay, typename Job3<CLAZZ, T>::Delegate d, CLAZZ* c, T arg)
    {
		uint64_t us = getTime() + microsDelay;
		Job3<CLAZZ, T>* j = new Job3<CLAZZ, T>(us, d, c, arg);
        jobs.push(j);
    }
    
    template <typename CLAZZ, typename T>
    void postDelayed(uint64_t microsDelay, typename Job4<CLAZZ>::Delegate d, CLAZZ* c)
    {
		uint64_t us = getTime() + microsDelay;
		Job4<CLAZZ>* j = new Job4<CLAZZ>(us, d, c);
        jobs.push(j);
    }
    
    void postAtTime(uint64_t time, Job::Runnable d)
    {
        Job* j = new Job(time, d);
        jobs.push(j);
    }
    
    template <typename T>
    void postAtTime(uint64_t time, typename Job2<T>::Delegate d, T arg)
    {
        Job2<T>* j = new Job2<T>(time, d, arg);
        jobs.push(j);
    }
    
    template <typename CLAZZ, typename T>
    void postAtTime(uint64_t time, typename Job3<CLAZZ, T>::Delegate d, CLAZZ* c, T arg)
    {
        Job3<CLAZZ, T>* j = new Job3<CLAZZ, T>(time, d, c, arg);
        jobs.push(j);
    }
    
    template <typename CLAZZ, typename T>
    void postAtTime(uint64_t time, typename Job4<CLAZZ>::Delegate d, CLAZZ* c)
    {
        Job4<CLAZZ>* j = new Job4<CLAZZ>(time, d, c);
        jobs.push(j);
    }
    
    bool run()
    {
        if (jobs.data.size == 0)
            return false;
        IJob* j = jobs.pop();

		//printf("%lld\n", j->time - getTime());
		while (getTime() < j->time);

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
