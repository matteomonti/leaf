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

    myclass(const myclass & that) : i(that.i)
    {
        std :: cout << "Copying myclass!" << std :: endl;
    }

    myclass(myclass && that) : i(that.i)
    {
        std :: cout << "Moving myclass!" << std :: endl;
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
    auto x = variant <int, myclass> :: construct <myclass> (4, 4.44, 'q');
    auto y = x;
    auto z = std :: move(y);
}
