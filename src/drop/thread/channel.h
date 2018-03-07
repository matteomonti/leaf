// Forward declarations

namespace drop
{
    template <typename> class channel;
};

#if !defined(__forward__) && !defined(__drop__thread__channel__h)
#define __drop__thread__channel__h

// Libraries

#include <stddef.h>
#include <exception>

// Includes

#include "drop/data/optional.hpp"

namespace drop
{
    template <typename type> class channel
    {
        // Settings

        struct settings
        {
            static constexpr size_t base_chunk_alloc = 1024;
        };

        // Service nested classes

        class chunk
        {
            // Members

            optional <type> * _buffer;
            size_t _size;

            struct
            {
                volatile size_t write;
                volatile size_t read;
            } _cursor;

            volatile chunk * _next;
            volatile size_t _last;

        public:

            // Constructors

            chunk(const size_t &);

            // Destructors

            ~chunk();

            // Methods

            void push(const type &);
            optional <type> pop();
        };

        // Members

        chunk * _read;
        chunk * _write;

    public:

        // Constructors

        channel();

        // Methods

        void push(const type &);
        optional <type> pop();
    };
};

#endif
