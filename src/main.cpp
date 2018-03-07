#include <iostream>
#include <thread>

#include <queue>

int main()
{
    std :: priority_queue <int> my_queue;

    for(int i = 0; i < 100000000; i++)
    {
        if(i % 1000000 == 0)
            std :: cout << i << std :: endl;

        my_queue.push(rand());
    }

    int total = 0;

    for(size_t i = 0; i < 100000000; i++)
    {
        int value = my_queue.top();
        my_queue.pop();

        if(i % 1000000 == 0)
        {
            std :: cout << i << std :: endl;
            total += value;
        }
    }

    std :: cout << total << std :: endl;
}
