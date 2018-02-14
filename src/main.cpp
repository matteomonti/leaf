#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/crypto/hash.hpp"
#include "drop/data/buffer.hpp"

using namespace drop;

int main()
{
    hasher my_hasher;
    my_hasher.update(44, 55, buffer("Hello World!"));
    std :: cout << my_hasher.finalize() << std :: endl;
}
