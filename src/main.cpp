#include <iostream>

#include "drop/data/optional.hpp"

using namespace drop;

class myclass
{
    int _i;

public:

    myclass()
    {
    }

    myclass(int i) : _i(i)
    {
    }

    void f()
    {
        std :: cout << "Hello from myclass! " << _i << std :: endl;
    }
};

int main()
{
    optional <int> x;

    if(!x)
        std :: cout << "So far so good" << std :: endl;

    x = 44;

    if(x)
        std :: cout << (*x) << std :: endl;

    optional <myclass> y = def;

    if(y)
        std :: cout << "Got it" << std :: endl;

    y = null;

    if(!y)
        std :: cout << "Also good" << std :: endl;

    y = myclass(33);

    if(y)
        y->f();
}
