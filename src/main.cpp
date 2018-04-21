#include <iostream>
#include <array>

#include "drop/async/promise.hpp"

using namespace drop;

int main()
{
    {
        promise <void> d;
        {
            promise <void> c;
            {
                promise <void> b;
                {
                    promise <void> a;

                    all(a, b, c, d).then([]()
                    {
                        std :: cout << "Completed!" << std :: endl;
                    }).except([](const std :: exception_ptr & exception)
                    {
                        std :: cout << "There was an exception!" << std :: endl;
                    });

                    a.resolve();
                }
                b.resolve();
            }
            c.resolve();
        }
        d.resolve();
    }
}
