#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/bytewise/bytewise.hpp"

using namespace drop;

int main()
{
    std :: cout << std :: get <0> (bytewise :: deserialize <int, int, int> (bytewise :: serialize(44, 55, 66))) << std :: endl;
}
