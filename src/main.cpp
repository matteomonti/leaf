#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/crypto/box.hpp"
#include "drop/bytewise/bytewise.hpp"

using namespace drop;

int main()
{
    box my_box;
    std :: cout << my_box.publickey() << std :: endl;
    std :: cout << my_box.secretkey() << std :: endl;
}
