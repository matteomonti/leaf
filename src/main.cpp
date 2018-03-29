#include <iostream>

#include "drop/async/eventemitter.hpp"

using namespace drop;

class myevent;

class myclass : public eventemitter <myevent>
{
public:

    void doit()
    {
        this->emit <myevent> ();
    }
};

int main()
{
    myclass myobject;

    std :: cout << "First call" << std :: endl;
    myobject.doit();

    myobject.on <myevent> ([]()
    {
        std :: cout << "Event triggered!!" << std :: endl;
    });

    std :: cout << "Second call" << std :: endl;
    myobject.doit();

    myobject.on <myevent> ([]()
    {
        std :: cout << "Also here!" << std :: endl;
    });

    std :: cout << "Third call" << std :: endl;
    myobject.doit();

    myobject.clear();

    std :: cout << "Fourth call" << std :: endl;
    myobject.doit();
}
