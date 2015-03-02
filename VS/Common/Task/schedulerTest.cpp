#include "Scheduler.h"

#include <stdio.h>

void print()
{
    printf("Hello\n");
}

void print(int foo)
{
    printf("Foo = %d\n", foo);
}

void print(float bar)
{
    printf("Bar = %f\n", bar);
}

class NoWay
{
public:
    void print()
    {
        printf("No Effing Way\n");
    }

    void print(int foo)
    {
        printf("NoWayFoo = %d\n", foo);
    }
};


int main(int argc, char** argv)
{
    Scheduler tasks;
    
    tasks.post(print);
    
    tasks.postDelayed(1000, print);
    
    tasks.postDelayed(1000, print, 7);
    
    tasks.postDelayed(1000, print, 7.0f);
    
    NoWay foo;
    tasks.postDelayed(2000, &NoWay::print, foo, 7);
    
    while (tasks.run());
}