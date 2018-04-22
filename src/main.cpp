#include <iostream>

#include "drop/async/collector.hpp"

using namespace drop;

template <typename type> void print()
{
    std :: cout << __PRETTY_FUNCTION__ << std :: endl;
}

struct asd
{
    int i;
    ~asd(){std :: cout << i << std :: endl;}
};

int main()
{
    {
        std :: array <promise <double>, 2> my_double_promises;
        {
            promise <int> my_int_promise;
            {
                promise <void> my_void_promise;

                collect(required(my_void_promise), my_int_promise, until(my_double_promises)).then([](const auto & collector)
                {
                    std :: cout << "Collection completed!" << std :: endl;

                    try
                    {
                        collector.template get <0> ();
                        std :: cout << "First promise resolved" << std :: endl;
                    }
                    catch(const char * message)
                    {
                        std :: cout << "First promise rejected: " << message << std :: endl;
                    }
                    catch(...)
                    {
                        std :: cout << "First promise pending" << std :: endl;
                    }

                    try
                    {
                        const int & value = collector.template get <1> ();
                        std :: cout << "Second promise resolved with " << value << std :: endl;
                    }
                    catch(const char * message)
                    {
                        std :: cout << "Second promise rejected: " << message << std :: endl;
                    }
                    catch(...)
                    {
                        std :: cout << "Second promise pending" << std :: endl;
                    }

                    try
                    {
                        const double & value = collector.template get <2> (0);
                        std :: cout << "Third promise resolved with " << value << std :: endl;
                    }
                    catch(const char * message)
                    {
                        std :: cout << "Third promise rejected: " << message << std :: endl;
                    }
                    catch(...)
                    {
                        std :: cout << "Third promise pending" << std :: endl;
                    }

                    try
                    {
                        const double & value = collector.template get <2> (1);
                        std :: cout << "Fourth promise resolved with " << value << std :: endl;
                    }
                    catch(const char * message)
                    {
                        std :: cout << "Fourth promise rejected: " << message << std :: endl;
                    }
                    catch(...)
                    {
                        std :: cout << "Fourth promise pending" << std :: endl;
                    }
                }).except([](const std :: exception_ptr & exception)
                {
                    try
                    {
                        std :: rethrow_exception(exception);
                    }
                    catch(const char * message)
                    {
                        std :: cout << "There was an exception: " << message << std :: endl;
                    }
                    catch(...)
                    {
                        std :: cout << "There was an exception: required missing" << std :: endl;
                    }
                });

                std :: cout << "Resolving void" << std :: endl;
                my_void_promise.resolve();
            }

            std :: cout << "Rejecting int" << std :: endl;
            my_int_promise.reject("hoho");
        }

        std :: cout << "Resolving first double" << std :: endl;
        my_double_promises[0].resolve(4.44);

        std :: cout << "Resolving second double" << std :: endl;
        my_double_promises[1].resolve(5.55);
    }

    std :: cout << "Shutting down" << std :: endl;
}
