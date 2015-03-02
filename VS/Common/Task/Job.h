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
//
//template <typename IN, typename OUT>
//class Job
//{
//public:
//    typedef OUT (*Delegate)(IN arg);
//    Delegate d;
//    long val;
//
//    Job(long val, Delegate d):val(val),d(d){}
//
//    OUT operator()(IN arg)
//    {
//        return d(arg);
//    }
//};

