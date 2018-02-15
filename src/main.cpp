#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/data/buffer.hpp"
#include "drop/crypto/secretbox.hpp"

using namespace drop;

int main()
{
    secretbox :: nonce my_nonce = secretbox :: nonce :: random();

    while(true)
        std :: cout << my_nonce++ << std :: endl;
}
