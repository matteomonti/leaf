#include <iostream>
#include <iomanip>

#include "drop/bytewise/bytewise.hpp"
#include "drop/data/buffer.hpp"

using namespace drop;

struct visitor
{
    void update(const uint8_t * data, const size_t & size)
    {
        for(size_t i = 0; i < size; i++)
            std :: cout << std :: hex << std :: setw(2) << std :: setfill('0') << (unsigned int) data[i] << " " << std :: dec;
    }
};

int main()
{
    visitor my_visitor;
    bytewise :: visit(my_visitor, buffer("Hello World!"));
    std :: cout << std :: endl;
}
