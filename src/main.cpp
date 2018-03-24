#include <iostream>

#include "poseidon/brahms/sampler.h"

using namespace drop;
using namespace poseidon;

int main()
{
    sampler my_sampler;

    while(true)
    {
        if(my_sampler.value())
            std :: cout << (*my_sampler.value()) << std :: endl;
        else
            std :: cout << "<null>" << std :: endl;
            
        my_sampler.next(signer().publickey());
    }
}
