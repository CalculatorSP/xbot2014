#pragma once

class IJob
{
    public:
    long time;
    IJob(long time):time(time){}
    virtual ~IJob(){}
    virtual void operator()() = 0;
};

class Job : public IJob
{
public:
    typedef void (*Runnable)();
    Runnable r;
    
    Job(long time, Runnable r)
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
    
    Job2(long time, Delegate d, T arg):IJob(time),d(d),arg(arg){}
    
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
    CLAZZ c;
    T arg;
    
    Job3(long time, Delegate d, CLAZZ c, T arg):IJob(time),d(d),arg(arg){}
    
    void operator()()
    {
        (c.*(d))(arg);
    }
};

template <typename CLAZZ>
class Job4 : public IJob
{
public:
    typedef void (CLAZZ::*Runnable)();
    Runnable r;
    CLAZZ c;
    long val;

    Job4(long val, Runnable r, CLAZZ c):val(val),r(r), c(c){}

    void operator()()
    {
        (c.*(r))();
    }
};
