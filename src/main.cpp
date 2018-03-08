#include <iostream>

#include "drop/network/address.hpp"

using namespace drop;

int main()
{
    address my_address("google.it", 1234);
    std :: cout << my_address << std :: endl;
    std :: cout << bytewise :: serialize(my_address) << std :: endl;
}
