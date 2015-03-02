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


int main(int argc, char** argv)
{
    Scheduler tasks(100);
    
    tasks.post(print);
    
    tasks.postDelayed(1000, print);
    
    tasks.postDelayed(1000, print, 7);
    
    tasks.postDelayed(1000, print, 7.0f);
    
    while (tasks.run());
}