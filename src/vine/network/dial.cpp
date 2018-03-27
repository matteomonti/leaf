// Includes

#include "dial.h"

namespace vine
{
    using namespace drop;

    // Constructors

    dial :: dial(const vine :: identifier & identifier, const connection & connection) : _identifier(identifier), drop :: connection(connection)
    {
    }

    // Getters

    const identifier & dial :: identifier() const
    {
        return this->_identifier;
    }
};
