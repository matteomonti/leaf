#include <iostream>

#include "drop/async/promise.hpp"

using namespace drop;

int main()
{
    {
        promise <int> my_promise;

        my_promise.then([](const int & value)
        {
            std :: cout << "The value is " << value << std :: endl;
        });

        my_promise.resolve(44);
    }

    {
        promise <int> my_promise;

        my_promise.then([](const int & value)
        {
            std :: cout << "The value is " << value << std :: endl;
        }).except([](const std :: exception_ptr & exception)
        {
            std :: cout << "There has been an exception: ";

            try
            {
                std :: rethrow_exception(exception);
            }
            catch(const char * exception)
            {
                std :: cout << exception << std :: endl;
            }
        });

        my_promise.reject("something went wrong! (Joking, everything's fine!)");
    }

    {
        promise <int> my_promise;
        promise <int> my_other_promise;

        my_promise.then([&](const int & value)
        {
            std :: cout << "The value is " << value << std :: endl;
            return my_other_promise;
        }).then([](const int & value)
        {
            std :: cout << "The next value is " << value << std :: endl;
        });

        my_other_promise.resolve(55);
        my_promise.resolve(44);
    }

    {
        promise <int> my_promise;
        promise <int> my_other_promise;
        promise <int> my_last_promise;

        my_promise.then([&](const int & value)
        {
            std :: cout << "The value is " << value << std :: endl;
            return my_other_promise.then([&](const int & value)
            {
                std :: cout << "The next value is " << value << std :: endl;
                return my_last_promise;
            });
        }).then([](const int & value)
        {
            std :: cout << "The last value is " << value << std :: endl;
        });

        my_last_promise.resolve(66);
        my_other_promise.resolve(55);
        my_promise.resolve(44);
    }

    {
        promise <int> my_promise;

        my_promise.then([](const int & value)
        {
            std :: cout << "First callback: " << value << std :: endl;
        });

        my_promise.then([](const int & value)
        {
            std :: cout << "Second callback: " << value << std :: endl;
        });

        my_promise.resolve(33);
    }

    {
        promise <int> my_promise;

        my_promise.then([](const int &)
        {
            return promise <int> ();
        }).then([](const int &)
        {
            return promise <int> ();
        }).then([](const int &)
        {
            return promise <const char *> ();
        }).then([](const char *)
        {
        }).except([](const std :: exception_ptr & exception)
        {
            std :: cout << "There has been an exception: ";

            try
            {
                std :: rethrow_exception(exception);
            }
            catch(const char * exception)
            {
                std :: cout << exception << std :: endl;
            }
        });

        my_promise.reject("something went wrong! (Joking, everything's fine!)");
    }

    {
        promise <void> my_promise;
        promise <int> my_other_promise;

        my_promise.then([&]()
        {
            std :: cout << "Good morning!" << std :: endl;
            return my_other_promise;
        }).then([](const int & value)
        {
            std :: cout << "The value is " << value << std :: endl;
        });

        my_promise.resolve();
        my_other_promise.resolve(44);

        std :: cout << "Good night" << std :: endl;
    }
}
