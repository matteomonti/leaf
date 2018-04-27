#ifndef __vine__dialers__directory__hpp
#define __vine__dialers__directory__hpp

// Includes

#include "directory.h"

namespace vine :: dialers
{
    using namespace drop;

    // client

    // Traits

    template <typename ctype> constexpr bool directory :: client :: traits :: is_dial_callable()
    {
        return sfinae :: is_valid([](auto && callback) -> decltype(callback(std :: declval <const dial &> ())) {}).template satisfied <const ctype> ();
    }

    // Constraints

    template <typename ctype> constexpr bool directory :: client :: constraints :: callback()
    {
        return traits :: is_dial_callable <ctype> ();
    }

    // Methods

    template <typename event, typename lambda, std :: enable_if_t <(std :: is_same <event, dial> :: value) && (directory :: client :: constraints :: callback <lambda> ())> *> void directory :: client :: on(const lambda & handler)
    {
        this->_acceptor.on <connection> ([=](connection sync_connection) -> promise <void>
        {
            try
            {
                std :: cout << "[directory / on] Connection incoming. Binding it to the pool." << std :: endl;
                pool :: connection connection = this->_pool.bind(sync_connection);

                std :: cout << "[directory / on] Receiving identifier." << std :: endl;
                signature :: publickey identifier = co_await connection.receive <signature :: publickey> ();

                std :: cout << "[directory / on] Receiving publickey." << std :: endl;
                class keyexchanger :: publickey publickey = co_await connection.receive <class keyexchanger :: publickey> ();

                std :: cout << "[directory / on] Receiving timestamp." << std :: endl;
                timestamp timestamp = co_await connection.receive <class timestamp> ();

                std :: cout << "[directory / on] Receiving signature." << std :: endl;
                signature signature = co_await connection.receive <class signature> ();

                std :: cout << "[directory / on] Verifying signature." << std :: endl;
                verifier verifier(identifier);
                verifier.verify(signature, signatures :: entry, publickey, timestamp);

                std :: cout << "[directory / on] Authenticating." << std :: endl;
                co_await connection.authenticate(this->_keyexchanger, publickey);

                std :: cout << "[directory / on] Calling handler " << &handler << "." << std :: endl;
                handler(dial(identifier, sync_connection));
            }
            catch(const std :: exception & exception)
            {
                std :: cout << "[directory / on] Exception: " << exception.what() << std :: endl;
            }
            catch(...)
            {
                std :: cout << "[directory / on] Unknown exception" << std :: endl;
            }
        });
    }
};

#endif
