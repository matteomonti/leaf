// Forward declarations

namespace drop
{
    class connection;
};

#if !defined(__forward__) && !defined(__drop__network__connection__h)
#define __drop__network__connection__h

// Libraries

#include <mutex>
#include <memory>

// Forward includes

#define __forward__
#include "pool.h"
#undef __forward__

// Includes

#include "drop/network/sockets/tcp.hpp"
#include "drop/data/optional.hpp"
#include "drop/data/buffer.hpp"
#include "drop/data/varint.hpp"
#include "drop/data/variant.hpp"
#include "drop/crypto/secretbox.hpp"
#include "drop/crypto/keyexchanger.h"

namespace drop
{
    class connection
    {
        // Friends

        friend class pool;

    public:

        // Constraints

        struct constraints
        {
            template <typename stype> static constexpr bool socket();
        };

    private:

        // Service nested classes

        class arc
        {
            // Friends

            friend class pool;
            
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

            struct
            {
                optional <secretbox> transmit;
                optional <secretbox> receive;
            } _secretchannel;

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
            template <typename... types> void send(const types & ...);

            buffer receive();
            template <typename... types> auto receive();

            void authenticate(const keyexchanger &, const class keyexchanger :: publickey &);

        private:

            // Private methods

            void send_init(const buffer &);
            bool send_step();

            void receive_init();
            bool receive_step();
            buffer receive_yield();
        };

        // Members

        std :: shared_ptr <arc> _arc;

    public:

        // Constructors

        template <typename stype, std :: enable_if_t <constraints :: socket <stype> ()> * = nullptr> connection(const stype &);

        // Methods

        void send(const buffer &) const;
        template <typename... types, std :: enable_if_t <(... && (bytewise :: constraints :: serializable <types> ()))> * = nullptr> void send(const types & ...) const;

        buffer receive() const;
        template <typename... types, std :: enable_if_t <(... && (bytewise :: constraints :: deserializable <types> ()))> * = nullptr> auto receive() const;

        void authenticate(const keyexchanger &, const class keyexchanger :: publickey &) const;
    };
};

#endif