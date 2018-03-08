// Forward declarations

namespace drop
{
    namespace sockets
    {
        class udp;
    };
};

#if !defined(__forward__) && !defined(__drop__network__sockets__udp__h)
#define __drop__network__sockets__udp__h

// Libraries

#include <arpa/inet.h>
#include <unistd.h>
#include <stddef.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>

// Includes

#include "exceptions.h"
#include "drop/network/address.h"
#include "drop/chrono/time.h"

namespace drop :: sockets
{
    class udp
    {
    public:

        // Settings

        struct settings
        {
            static constexpr size_t mtu = 2048;
        };

        // Nested classes

        class packet
        {
            // Friends

            friend class udp;

            // Members

            address _remote;
            uint8_t _message[settings :: mtu + 1];
            size_t _size;

        public:

            // Getters

            const address & remote() const;
            const uint8_t * message() const;
            const size_t & size() const;

            // Casting

            operator bool() const;
        };

    private:

        // Members

        int _descriptor;
        class address :: port _port;
        bool _blocking;

    public:

        // Constructors

        udp();

        // Getters

        const int & descriptor() const;

        // Setters

        void send_timeout(const interval &);
        void receive_timeout(const interval &);

        void block(const bool &);

        // Methods

        void bind(const class address :: port &);

        bool send(const address &, const uint8_t *, const size_t &);
        packet receive();

        void close();
    };
};

#endif
