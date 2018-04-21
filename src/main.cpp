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
                    std :: array <promise <int>, 4> promises = {a, b, c, d};

                    all(promises).then([](const std :: array <int, 4> & values)
                    {
                        std :: cout << "Completed! Values are " << values[0] << ", " << values[1] << ", " << values[2] << ", " << values[3] << std :: endl;
                    }).except([](const std :: exception_ptr & exception)
                    {
                        std :: cout << "There was an exception!" << std :: endl;
                    });

                    a.resolve(33);
                }
                b.resolve(99);
            }
            c.resolve(66);
        }
        d.resolve(44);
    }
}
