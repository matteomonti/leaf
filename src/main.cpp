#include <iostream>
#include <iomanip>

#include "drop/bytewise/bytewise.hpp"

using namespace drop;

struct visitor
{
    void update(const uint8_t * data, const size_t & size)
    {
        for(size_t i = 0; i < size; i++)
            std :: cout << std :: hex << std :: setw(2) << std :: setfill('0') << (unsigned int) data[i] << " " << std :: dec;
    }
};

struct acceptor
{
    int i;
    long int j;

    acceptor(const int & i, const int & j) : i(i), j(j)
    {
    }

    template <typename vtype> void accept(bytewise :: visitor <vtype> & visitor) const
    {
        visitor << this->i << this->j;
    }
};

int main()
{
    visitor my_visitor;

    bytewise :: visit(my_visitor, 44);
    std :: cout << std :: endl << std :: endl;

    int x[3][3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    bytewise :: visit(my_visitor, x);
    std :: cout << std :: endl << std :: endl;

    acceptor my_acceptor(33, 99);
    bytewise :: visit(my_visitor, my_acceptor);
    std :: cout << std :: endl << std :: endl;
}
