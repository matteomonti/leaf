// Forward declarations

namespace drop
{
    template <typename...> class messenger;
    struct close;
};

#if !defined(__forward__) && !defined(__drop__distributed__messenger__h)
#define __drop__distributed__messenger__h

// Libraries

#include <memory>

// Includes

#include "drop/network/connection.hpp"
#include "drop/network/pool.hpp"
#include "drop/data/variant.hpp"
#include "drop/async/pipe.hpp"
#include "drop/chrono/crontab.h"
#include "drop/chrono/time.hpp"
#include "drop/async/eventemitter.hpp"

namespace drop
{
    template <typename... types> class messenger
    {
        // Settings

        struct settings
        {
            static constexpr interval keepalive = 5_s;
        };

    public:

        // Exceptions

        struct exceptions
        {
            class messenger_deleted : public std :: exception
            {
                const char * what() const throw();
            };
        };

    private:

        // Service nested classes

        struct arc : public eventemitter <types, types>..., public eventemitter <close>
        {
            // Friends

            template <typename...> friend class messenger;

            // Public members

            pool :: connection connection;
            pipe <variant <types...>> pipe;

            bool alive;
            timestamp lastsend;

            std :: mutex mutex;

            crontab & crontab;

            // Constructors

            arc(const pool :: connection &, class crontab &);
        };

        // Members

        std :: shared_ptr <arc> _arc;

    public:

        // Constructors

        messenger(const pool :: connection &, crontab &);

        // Methods

        template <typename type, typename lambda, std :: enable_if_t <(... || (std :: is_same <type, types> :: value)) && (eventemitter <type, type> :: constraints :: template callback <lambda> ())> * = nullptr> void on(const lambda &);
        template <typename type, std :: enable_if_t <(... || (std :: is_same <type, types> :: value))> * = nullptr> void send(const type &);

        template <typename type, typename lambda, std :: enable_if_t <std :: is_same <type, close> :: value && eventemitter <close> :: constraints :: template callback <lambda> ()> * = nullptr> void on(const lambda &);

        void start();

    private:

        // Static private methods

        static promise <void> send(std :: shared_ptr <arc>);
        static promise <void> receive(std :: shared_ptr <arc>);
        static promise <void> keepalive(std :: shared_ptr <arc>);
    };

    // Events

    struct close {};
};

#endif
