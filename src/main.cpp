#include <iostream>

#include "drop/async/collector.hpp"

using namespace drop;

template <typename type> void print()
{
    std :: cout << __PRETTY_FUNCTION__ << std :: endl;
}

int main()
{
    promise <void> my_void_promise;
    promise <int> my_int_promise;
    std :: array <promise <double>, 2> my_double_promises;

    collect(my_void_promise, my_int_promise, my_double_promises).then([](const auto & collector)
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

        try
        {
            const int & value = collector.template get <1> ();
            std :: cout << "Second promise resolved with " << value << std :: endl;
        }
        catch(const char * message)
        {
            std :: cout << "Second promise rejected: " << message << std :: endl;
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

        try
        {
            const double & value = collector.template get <2> (1);
            std :: cout << "Fourth promise resolved with " << value << std :: endl;
        }
        catch(const char * message)
        {
            std :: cout << "Fourth promise rejected: " << message << std :: endl;
        }
    });

    my_void_promise.reject("hihi");
    my_int_promise.reject("hoho");
    my_double_promises[0].reject("lmfao");
    my_double_promises[1].reject("boobidoobi");
}
