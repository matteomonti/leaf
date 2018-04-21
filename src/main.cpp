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

                    first(std :: array <promise <int>, 4> {a, b, c, d}).then([](const int & value)
                    {
                        std :: cout << "Completed: " << value << "!" << std :: endl;
                    }).except([](const std :: exception_ptr & exception)
                    {
                        try
                        {
                            std :: rethrow_exception(exception);
                        }
                        catch(const exceptions :: multiple & multiple)
                        {
                            std :: cout << "There was a multiple exception:" << std :: endl;
                            for(const std :: exception_ptr & single : multiple.exceptions)
                                try
                                {
                                    std :: rethrow_exception(single);
                                }
                                catch(const char * message)
                                {
                                    std :: cout << message << std :: endl;
                                }
                        }
                    });

                    std :: cout << "Rejecting first" << std :: endl;
                    a.reject("First reject");
                }

                std :: cout << "Rejecting second" << std :: endl;
                b.reject("Second reject");
            }

            std :: cout << "Resolving third" << std :: endl;
            c.resolve(3);
        }

        std :: cout << "Rejecting fourth" << std :: endl;
        d.reject("Fourth reject");
    }
}
