#include <iostream>
#include <thread>

#include "drop/data/heap.hpp"

using namespace drop;

int main()
{
    heap <int> my_heap;

    for(int i = 0; i < 100000000; i++)
    {
        if(i % 1000000 == 0)
            std :: cout << i << std :: endl;

        my_heap.push(rand());
    }

    int total = 0;

    for(int i = 0; i < 100000000; i++)
    {
        int value = my_heap.pop();

        if(i % 1000000 == 0)
        {
            std :: cout << i << std :: endl;
            total += value;
        }
    }

    std :: cout << total << std :: endl;
}
