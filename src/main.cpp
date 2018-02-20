#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/crypto/hash.hpp"
#include "drop/data/buffer.hpp"

using namespace drop;

int main()
{
    auto my_key = hash :: key :: random();

    std :: cout << my_key << std :: endl << std :: endl;

    std :: cout << hash(1, 2, 3) << std :: endl;
    std :: cout << hash :: keyed(my_key, 1, 2, 3) << std :: endl;
    std :: cout << hash :: keyed(my_key, 1, 2) << std :: endl;
    std :: cout << hash :: keyed(my_key, 1, 2, 3) << std :: endl;
}
