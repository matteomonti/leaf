#include <iostream>

#include "drop/bytewise/bytewise.hpp"

using namespace drop;

int main()
{
    std :: array <int, 3> x{1, 2, 3};
    buffer serialized = bytewise :: serialize(x);
    std :: array <int, 3> y = bytewise :: deserialize <std :: array <int, 3>> (serialized);

    std :: cout << y[0] << ", " << y[1] << ", " << y[2] << std :: endl;
}
