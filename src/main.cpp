#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/crypto/sha256.hpp"

using namespace drop;

int main()
{
    sha256 my_sha;
    my_sha.update(3, 4, 5, 6);
    sha256 :: digest my_digest = my_sha;

    std :: cout << my_digest << std :: endl;

    std :: cout << sha256(buffer("Hello world!")) << std :: endl;
}
