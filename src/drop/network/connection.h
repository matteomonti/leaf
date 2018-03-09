// Forward declarations

namespace drop
{
    class connection;
};

#if !defined(__forward__) && !defined(__drop__network__connection__h)
#define __drop__network__connection__h

// Libraries

#include <mutex>

// Includes

#include "drop/network/sockets/tcp.hpp"
#include "drop/data/buffer.hpp"
#include "drop/data/varint.hpp"
#include "drop/data/variant.hpp"

namespace drop
{
    class connection
    {
    public: // REMOVE ME

        // Service nested classes

        class arc
        {
            // Members

            variant <sockets :: tcp> _socket;

            struct
            {
                buffer buffer;
                size_t cursor;
            } _write;

            variant <varint :: streamer, buffer :: streamer> _read;

            struct
            {
                std :: mutex send;
                std :: mutex receive;
            } _mutex;

        public:

            // Constructors

            template <typename stype> arc(const stype &);

            // Destructor

            ~arc();

            // Getters

            int descriptor() const;

            // Setters

            void block(const bool &);

            // Methods

            void send(const buffer &);
            buffer && receive();

        private:

            // Private methods

            void send_init(const buffer &);
            bool send_step();

            void receive_init();
            bool receive_step();
            buffer && receive_yield();
        };
    };
};

#endif
