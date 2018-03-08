// Forward declarations

namespace drop
{
    namespace sockets
    {
        class tcp;
    };
};

#if !defined(__forward__) && !defined(__network__sockets__tcp__h)
#define __network__sockets__tcp__h

// Libraries

#include <arpa/inet.h>
#include <unistd.h>
#include <stddef.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

// Includes

#include "exceptions.h"
#include "drop/network/address.h"
#include "drop/chrono/time.hpp"

namespace drop :: sockets
{
    class tcp
    {
    public:

        // Settings

        struct settings
        {
            struct listen
            {
                static constexpr size_t slots = 128;
            };
        };

    private:

        // Members

        int _descriptor;

        class address :: port _port;
        address _remote;

        bool _blocking;

    public:

        // Constructors

        tcp();

    private:

        // Private constructors

        tcp(const int &, const class address :: port &, const address &);

    public:

        // Getters

        const int & descriptor() const;
        const address & remote() const;

        // Setters

        void send_timeout(const interval &);
        void receive_timeout(const interval &);

        void block(const bool &);

        // Methods

        void bind(const class address :: port &);
        void connect(const address &);

        void listen();
        tcp accept();

        size_t available();

        size_t send(const uint8_t *, const size_t &);
        size_t receive(uint8_t *, const size_t &);

        void close();
    };
};

#endif
