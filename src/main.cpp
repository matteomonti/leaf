#include <iostream>

#include "drop/async/promise.hpp"

using namespace drop;

promise <void> my_promise;

promise <void> f()
{
    try
    {
        co_await my_promise;
    }
    catch(const char * message)
    {
        std :: cout << "Exception: " << message << std :: endl;
    }

    co_return;
}

int main()
{
    f().except([](const std :: exception_ptr & exception)
    {
        try
        {
            std :: rethrow_exception(exception);
        }
        catch(const int & number)
        {
            std :: cout << "Exception: " << number << std :: endl;
        }
    });

    my_promise.reject(44); //.reject("Something went wrong!!");
}
