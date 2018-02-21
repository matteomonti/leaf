#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

// #include "drop/data/variant.hpp"

// using namespace drop;

#define typeswitch(type)
#define typecase(type)

int main()
{
    typedef int x;
    std :: cout << sizeof(x) << std :: endl;
    {
        typedef double x;
        std :: cout << sizeof(x) << std :: endl;
    }

    /*typeswitch(int)
    {
        typecase(int)
        {

        }
    }*/
}
