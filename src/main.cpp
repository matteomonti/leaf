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

    myclass()
    {
    }

    myclass(int i, double, char) : i(i)
    {
    }

    // Destructor

    ~myclass()
    {
    }

    // Methods

    template <typename vtype> void accept(bytewise :: reader <vtype> & visitor) const
    {
        visitor << (this->i);
    }

    template <typename vtype> void accept(bytewise :: writer <vtype> & visitor)
    {
        visitor >> (this->i);
    }

    void f()
    {
        std :: cout << "Hello from myclass!" << std :: endl;
    }
};


int main()
{
    auto x = variant <int, myclass> :: construct <myclass> (4, 4.44, 'q');
    buffer y = bytewise :: serialize(x);
    std :: cout << y << std :: endl << std :: endl;

    auto z = bytewise :: deserialize <variant <int, myclass>> (y);
    z.match([](myclass & z)
    {
        std :: cout << "Got a myclass!" << std :: endl;
        z.f();
        std :: cout << "Member i is " << z.i << std :: endl;
    });

    std :: cout << std :: endl << std :: endl;

    x = 73;
    y = bytewise :: serialize(x);
    std :: cout << y << std :: endl << std :: endl;

    z = bytewise :: deserialize <variant <int, myclass>> (y);
    z.match([](int & z)
    {
        std :: cout << "Got an int: " << z << std :: endl;
    });
}
