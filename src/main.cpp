#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/data/variant.hpp"
#include "drop/utils/enablers.h"

using namespace drop;

class myclass
{
public:

    // Constructors

    myclass(int, double, char)
    {
    }

    void f()
    {
        std :: cout << "Hello from myclass!" << std :: endl;
    }
};

int main()
{
    auto x = variant <int, myclass> :: construct <myclass> (1, 4.44, 'q');

    x.match([](myclass & x)
    {
        std :: cout << "Got a myclass!" << std :: endl;
        x.f();
    });
}
