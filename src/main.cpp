#include <iostream>
#include <iomanip>

#include "drop/bytewise/bytewise.hpp"
#include "drop/data/buffer.hpp"

using namespace drop;

struct acceptor
{
    uint32_t i;
    uint64_t j;
    buffer b;

    template <typename vtype> void accept(bytewise :: reader <vtype> & visitor) const
    {
        visitor << this->i << this->j << this->b;
    }

    template <typename vtype> void accept(bytewise :: writer <vtype> & visitor)
    {
        visitor >> this->i >> this->j >> this->b;
    }
};

int main()
{
    buffer bytes = bytewise :: serialize(33, uint64_t(44), acceptor{.i = 33, .j = 99, .b = "Hello World!"});
    std :: cout << bytes << std :: endl;

    std :: tuple <uint32_t, uint64_t, acceptor> restored = bytewise :: deserialize <uint32_t, uint64_t, acceptor> (bytes);

    std :: cout << std :: get <2> (restored).i << std :: endl;
    std :: cout << std :: get <2> (restored).j << std :: endl;
    std :: cout << std :: get <2> (restored).b << std :: endl;
}
