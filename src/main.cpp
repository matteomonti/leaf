#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/crypto/hash.hpp"
#include "drop/data/buffer.hpp"

using namespace drop;

int main()
{
    std :: cout << hash :: key :: random() << std :: endl;
}
