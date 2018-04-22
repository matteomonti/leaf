// Forward declarations

namespace drop
{
    template <typename...> class messenger;
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

namespace drop
{
    template <typename... types> class messenger
    {
        // Settings

        struct settings
        {
            static constexpr interval keepalive = 5_s;
        };

        // Service nested classes

        struct arc
        {
            // Public members

            pool :: connection connection;
            pipe <variant <types...>> pipe;

            bool alive;
            timestamp lastsend;

            std :: mutex mutex;

            crontab & crontab;

            // Constructors

            arc(const pool :: connection &, class crontab &);

            // Destructor

            ~arc() // REMOVE ME
            {
                std :: cout << "Deleting arc" << std :: endl;
            }
        };

        // Members

        std :: shared_ptr <arc> _arc;

    public:

        // Constructors

        messenger(const pool :: connection &, crontab &);

    private:

        // Static private methods

        static promise <void> send(std :: shared_ptr <arc>);
        static promise <void> receive(std :: shared_ptr <arc>);
        static promise <void> keepalive(std :: shared_ptr <arc>);
    };
};

#endif
