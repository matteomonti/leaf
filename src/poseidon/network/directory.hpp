#ifndef __poseidon__network__directory__hpp
#define __poseidon__network__directory__hpp

// Includes

#include "directory.h"

namespace poseidon
{
    using namespace drop;

    // client

    // Traits

    template <typename ctype> constexpr bool directory :: client :: traits :: is_connection_callable()
    {
        return sfinae :: is_valid([](auto && callback) -> decltype(callback(std :: declval <const connection &> ())) {}).template satisfied <const ctype> ();
    }

    // Constraints

    template <typename ctype> constexpr bool directory :: client :: constraints :: callback()
    {
        return traits :: is_connection_callable <ctype> ();
    }

    // Methods

    template <typename event, typename lambda, std :: enable_if_t <(std :: is_same <event, directory :: connection> :: value) && (directory :: client :: constraints :: callback <lambda> ())> *> void directory :: client :: on(const lambda & handler)
    {
        this->_acceptor.on <:: drop :: connection> ([=](:: drop :: connection sync_connection) -> promise <void>
        {
            try
            {
                pool :: connection connection = this->_pool.bind(sync_connection);

                signature :: publickey identifier = co_await connection.receive <signature :: publickey> ();
                class keyexchanger :: publickey publickey = co_await connection.receive <class keyexchanger :: publickey> ();
                signature signature = co_await connection.receive <class signature> ();

                verifier verifier(identifier);
                verifier.verify(signature, signatures :: entry, publickey);

                connection.authenticate(this->_keyexchanger, publickey);
                handler((class connection){sync_connection, identifier});
            }
            catch(...)
            {
            }
        });
    }
};

#endif
