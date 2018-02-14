#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/data/buffer.hpp"
#include "drop/crypto/secretbox.hpp"

using namespace drop;

int main()
{
    std :: cout << secretbox :: key :: random() << std :: endl;
}
