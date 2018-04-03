#include <iostream>
#include <fstream>

#include "drop/async/eventemitter.hpp"

using namespace drop;

class myevent
{
};

class myclass : public eventemitter <myevent>
{
public:

    void doit()
    {
        bool pass = this->emit <myevent> ();

        if(pass)
            std :: cout << "Event passed" << std :: endl << std :: endl;
        else
            std :: cout << "Event not passed" << std :: endl << std :: endl;
    }
};

int main()
{
    myclass myobject;
    myobject.doit();

    myobject.on <myevent> ([]()
    {
        std :: cout << "First callback" << std :: endl;
    });

    myobject.doit();

    bool pass = true;

    myobject.on <myevent> ([&]()
    {
        std :: cout << "Second callback" << std :: endl;
        return pass;
    });

    myobject.doit();

    myobject.on <myevent> ([]()
    {
        std :: cout << "Third callback" << std :: endl;
    });

    myobject.doit();

    pass = false;
    myobject.doit();

    pass = true;

    myobject.on <myevent> ([&]()
    {
        std :: cout << "Fourth callback" << std :: endl;
        throw "You shall not pass!";
    });

    myobject.doit();
}
