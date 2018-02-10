#include <iostream>
#include <iomanip>

#include "drop/bytewise/bytewise.hpp"
#include "drop/data/varint.hpp"

using namespace drop;

struct printer
{
    void update(const uint8_t * data, const size_t & size)
    {
        for(size_t i = 0; i < size; i++)
            std :: cout << std :: hex << std :: setw(2) << std :: setfill('0') << (unsigned int) data[i] << " " << std :: dec;
    }
};

int main()
{
    printer my_printer;
    bytewise :: visit(my_printer, varint(398579345));
    std :: cout << std :: endl;
}
