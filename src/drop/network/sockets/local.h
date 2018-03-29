// Forward declarations

namespace drop
{
    namespace sockets
    {
        class local;
        struct socketpair;
    };
};

#if !defined(__forward__) && !defined(__drop__network__sockets__local__h)
#define __drop__network__sockets__local__h

// Libraries

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

// Includes

#include "exceptions.h"
#include "drop/chrono/time.hpp"
#include "drop/network/address.h"
#include "traits.h"

namespace drop :: sockets
{
    class local
    {
        // Friends

        friend class socketpair;

    public:

        // Settings

        struct settings
        {
            struct streamer
            {
                static constexpr size_t buffer = 128;
            };
        };

    private:

        // Members

        int _descriptor;
        bool _blocking;

        // Private constructors

        local() = default;
        local(const int &);

    public:

        // Getters

        const int & descriptor() const;
        const address & remote() const;

        // Setters

        void send_timeout(const interval &);
        void receive_timeout(const interval &);

        void block(const bool &);

        // Methods

        size_t available();

        size_t send(const uint8_t *, const size_t &);

        size_t receive(uint8_t *, const size_t &);
        template <typename stype, std :: enable_if_t <constraints :: streamer <stype> ()> * = nullptr> bool receive(stype &);

        void close();
    };

    struct socketpair
    {
        // Constructors

        socketpair();

        // Public members

        local alpha;
        local beta;
    };
};

#endif
