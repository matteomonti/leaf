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

    // Members

    int i;

    // Constructors

    myclass(int, double, char)
    {
        std :: cout << "Creating myclass!" << std :: endl;
    }

    // Destructor

    ~myclass()
    {
        std :: cout << "Deleting myclass!" << std :: endl;
    }

    // Methods

    void f()
    {
        std :: cout << "Hello from myclass!" << std :: endl;
    }
};

int main()
{
    std :: cout << "Creating variant" << std :: endl;
    auto x = variant <int, myclass> :: construct <myclass> (1, 4.44, 'q');
    myclass & y = x.reinterpret <myclass> ();

    y.i = 55;

    x.match([](const myclass & x)
    {
        std :: cout << x.i << std :: endl;
    });

    std :: cout << "Deleting variant" << std :: endl;
}
