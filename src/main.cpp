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

                    first(std :: array <promise <void>, 4> {a, b, c, d}).then([]()
                    {
                        std :: cout << "Completed!" << std :: endl;
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

            std :: cout << "Rejecting third" << std :: endl;
            c.reject("Third reject");
        }

        std :: cout << "Resolving fourth" << std :: endl;
        d.reject("Fourth reject");
    }
}
