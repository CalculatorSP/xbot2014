#pragma once

#include <stdint.h>

class IJob
{
    public:
    uint64_t time;
    IJob(uint64_t time):time(time){}
    virtual ~IJob(){}
    virtual void operator()() = 0;
};

class Job : public IJob
{
public:
    typedef void (*Runnable)();
    Runnable r;
    
    Job(uint64_t time, Runnable r)
    :IJob(time), r(r){}
    
    void operator()()
    {
        r();
    }
};

template <typename T>
class Job2 : public IJob
{
public:
    typedef void (*Delegate)(T arg);
    Delegate d;
    T arg;
    
    Job2(uint64_t time, Delegate d, T arg):IJob(time),d(d),arg(arg){}
    
    void operator()()
    {
        d(arg);
    }
};

template <typename CLAZZ, typename T>
class Job3 : public IJob
{
public:
    typedef void (CLAZZ::*Delegate)(T arg);
    Delegate d;
    CLAZZ* c;
    T arg;
    
    Job3(uint64_t time, Delegate d, CLAZZ* c, T arg):IJob(time),d(d),c(c),arg(arg){}
    
    void operator()()
    {
        (c->*(d))(arg);
    }
};

template <typename CLAZZ>
class Job4 : public IJob
{
public:
    typedef void (CLAZZ::*Runnable)();
    Runnable r;
    CLAZZ* c;

    Job4(uint64_t time, Runnable r, CLAZZ* c):IJob(time),r(r), c(c){}

    void operator()()
    {
        (c->*(r))();
    }
};
