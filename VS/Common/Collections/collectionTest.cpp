#include "List.h"
#include "Heap.h"
#include <stdio.h>

int swapper(int parent, int child)
{
    if (parent <= child)
        return false;
    return true;
}

int main(int argc, char** argv)
{
    Heap<int> heap(100, swapper);
    
    for (int i = 0; i < 50; i++)
    {
        heap.push(i * 31 % 20);
//        printf("Pushing %d:\n", (i*2%20));
//        for (int j = 0; j < heap.size; j++)
//        {
//            printf("\t%d: %d\n", j, heap.data[j]);
//        }
    }
    
    for (int i = 0; i < 70; i++)
    {
        printf("%d\n", heap.pop());
    }
    
    
    printf("LIST!");
    List<float> list;
    for (int i = 0; i < 20; i++)
    {
        list.add(i*3%7);
    }
    
    for (int i = 0; i < 20; i++)
    {
        printf("%d: %f\n", i, list[i]);
    }
    
    for (int i = 0; i < 10;i+=2)
    {
        list.remove(i);
    }
    
    for (int i = 0; i < 15; i++)
    {
        printf("%f\n", list[i]);
    }
}