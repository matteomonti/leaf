#ifndef __drop__network__connection__hpp
#define __drop__network__connection__hpp

// Includes

#include "connection.h"

namespace drop
{
    // Constraints

    template <typename stype> constexpr bool connection :: constraints :: socket()
    {
        return std :: is_same <stype, sockets :: tcp> :: value || std :: is_same <stype, sockets :: local> :: value;
    }

    // arc

    // Constructors

    template <typename stype> connection :: arc :: arc(const stype & socket) : _socket(socket)
    {
        this->_semaphores.send.post();
        this->_semaphores.receive.post();
    }

    // Methods

    template <typename... types> void connection :: arc :: send(const types & ... messages)
    {
        this->send(bytewise :: serialize(messages...));
    }

    template <typename... types> auto connection :: arc :: receive()
    {
        buffer message = this->receive();
        return bytewise :: deserialize <types...> (message);
    }

    // connection

    // Constructors

    template <typename stype, std :: enable_if_t <connection :: constraints :: socket <stype> ()> *> connection :: connection(const stype & socket) : _arc(new arc(socket))
    {
    }

    // Methods

    template <typename... types, std :: enable_if_t <(... && (bytewise :: constraints :: serializable <types> ()))> *> void connection :: send(const types & ... messages) const
    {
        this->_arc->send(messages...);
    }

    template <typename... types, std :: enable_if_t <(... && (bytewise :: constraints :: deserializable <types> ()))> *> auto connection :: receive() const
    {
        return this->_arc->receive <types...> ();
    }
};

#endif
