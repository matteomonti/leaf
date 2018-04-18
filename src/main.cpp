#include <iostream>

#include "drop/data/syncset.hpp"

using namespace drop;

int main()
{
    syncset <int> my_syncset;

    for(int i = 0; i < 1024; i++)
        my_syncset.add(i);

    std :: cout << "Removing" << std :: endl;
    my_syncset.remove(1048576);

    std :: cout << "Adding" << std :: endl;
    my_syncset.add(44);

    std :: cout << "Checking present" << std :: endl;
    for(int i = 0; i < 1024; i++)
        if(!(my_syncset.find(i)))
            std :: cout << "ERROR!" << std :: endl;

    std :: cout << "Checking not present" << std :: endl;
    for(int i = 1024; i < 2048; i++)
        if(my_syncset.find(i))
            std :: cout << "ERROR!" << std :: endl;
}
