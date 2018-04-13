#include <iostream>

#include "drop/data/optional.hpp"
#include "drop/bytewise/bytewise.hpp"

using namespace drop;

class myclass
{
public:

    // Members

    uint32_t _i;

    // Constructors

    myclass()
    {
    }

    myclass(const uint32_t & i) : _i(i)
    {
    }

    // Methods

    template <typename vtype> void accept(bytewise :: reader <vtype> & reader) const
    {
        reader << this->_i;
    }

    template <typename vtype> void accept(bytewise :: writer <vtype> & writer)
    {
        writer >> this->_i;
    }
};

int main()
{
    optional <myclass> my_optional = 44;
    buffer serialized = bytewise :: serialize(my_optional);

    optional <myclass> my_other_optional = bytewise :: deserialize <optional <myclass>> (serialized);

    if(my_other_optional)
        std :: cout << my_other_optional->_i << std :: endl;
    else
        std :: cout << "<null>" << std :: endl;
}
