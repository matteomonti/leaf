#include <iostream>

// Includes

#include "drop/async/promise.hpp"
#include "drop/chrono/time.hpp"

using namespace drop;

promise <void> my_promise;
promise <void> my_other_promise;

promise <void> f()
{
    return my_promise.then([]()
    {
        return my_other_promise;
    });
}

promise <void> run()
{
    co_await f();
    std :: cout << "Done!" << std :: endl;
}

int main()
{
    run();

    sleep(1_s);
    std :: cout << "Solving my_promise" << std :: endl;
    my_promise.resolve();

    sleep(1_s);
    std :: cout << "Solving my_other_promise" << std :: endl;
    my_other_promise.resolve();

    sleep(1_h);
}
