#include <iostream>
#include <array>

#include "drop/async/promise.hpp"

using namespace drop;

int main()
{
    {
        promise <int> d;
        {
            promise <int> c;
            {
                promise <int> b;
                {
                    promise <int> a;

                    all(a, b, c, d).then([](const std :: array <int, 4> & values)
                    {
                        std :: cout << "Completed!" << std :: endl;
                    }).except([](const std :: exception_ptr & exception)
                    {
                        std :: cout << "There was an exception!" << std :: endl;
                    });

                    a.resolve(1);
                }
                b.resolve(2);
            }
            c.resolve(3);
        }
        d.resolve(4);
    }
}
