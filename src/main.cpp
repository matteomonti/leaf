#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/crypto/sha256.hpp"

using namespace drop;

int main()
{
    std :: cout << ((sha256 :: digest) sha256(3) == (sha256 :: digest) sha256(3)) << std :: endl;
}
