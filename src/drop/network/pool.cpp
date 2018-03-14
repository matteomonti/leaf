// Includes

#include "pool.hpp"

namespace drop
{
    // connection

    // Private constructors

    pool :: connection :: connection(pool & pool, const :: drop :: connection & connection) : _pool(pool), _arc(connection._arc)
    {
    }

    // Methods

    promise <void> pool :: connection :: send(const buffer & message) const
    {
        return this->_pool.send(this->_arc, message);
    }

    // pool

    // Methods

    pool :: connection pool :: bind(const :: drop :: connection & connection)
    {
        return (class connection){(*this), connection};
    }

    // Private methods

    promise <void> pool :: send(const std :: shared_ptr <:: drop :: connection :: arc> & arc, const buffer & message)
    {
        return promise <void> ();
    }
};
