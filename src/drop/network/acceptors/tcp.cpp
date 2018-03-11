// Includes

#include "tcp.h"

namespace drop :: acceptors
{
    // sync

    // Constructors

    tcp :: sync :: sync(const uint16_t & port)
    {
        this->_socket.bind(port);
        this->_socket.listen();
    }

    // Methods

    connection tcp :: sync :: accept()
    {
        return connection(this->_socket.accept());
    }
};
