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

    myclass(int i, double, char) : i(i)
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

    template <typename vtype> void accept(bytewise :: reader <vtype> & visitor) const
    {
        visitor << (this->i);
    }

    void f()
    {
        std :: cout << "Hello from myclass!" << std :: endl;
    }

    // Operators

    myclass & operator = (const myclass & rho)
    {
        std :: cout << "Assigning myclass!" << std :: endl;
        this->i = rho.i;
        return (*this);
    }

    myclass & operator = (myclass && rho)
    {
        std :: cout << "Move assigning myclass!" << std :: endl;
        this->i = rho.i;
        return (*this);
    }
};


int main()
{
    auto x = variant <int, myclass> :: construct <myclass> (4, 4.44, 'q');
    buffer y = bytewise :: serialize(x);
    std :: cout << y << std :: endl;
}
