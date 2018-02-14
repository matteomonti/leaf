// Forward declarations

namespace drop
{
    class hash;
    class hasher;
};

#if !defined(__forward__) && !defined(__drop__crypto__hash__h)
#define __drop__crypto__hash__h

// Libraries

#include <sodium.h>

// Includes

#include "drop/bytewise/bytewise.hpp"

namespace drop
{
    class hash
    {
    public:

        // Properties

        static constexpr size_t size = crypto_generichash_BYTES;

        // Nested classes

        class key
        {
        public:

            // Properties

            static constexpr size_t size = crypto_generichash_KEYBYTES;

        private:

            // Members

            uint8_t _bytes[size];

        public:

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
            template <typename vtype> void accept(bytewise :: writer <vtype> &);

            // Operators

            const uint8_t & operator [] (const size_t &) const;

            bool operator == (const key &) const;

            // Casting

            operator const uint8_t * () const;

            // Static methods

            static key random();
        };

    private:

        // Friends

        friend class hasher;

        // Members

        uint8_t _bytes[size];

    public:

        // Constructors

        hash() = default;
        template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, hasher> ()))> * = nullptr> hash(const atypes & ...);

    private:

        // Private constructors

        hash(crypto_generichash_state &);

    public:

        // Methods

        template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
        template <typename vtype> void accept(bytewise :: writer <vtype> &);

        // Operators

        const uint8_t & operator [] (const size_t &) const;

        bool operator == (const hash &) const;

        // Casting

        operator const uint8_t * () const;
    };

    // Ostream integration

    std :: ostream & operator << (std :: ostream &, const hash :: key &);
    std :: ostream & operator << (std :: ostream &, const hash &);

    class hasher
    {
        // Members

        crypto_generichash_state _state;

    public:

        // Constructors

        hasher();

        // Methods

        void update(const uint8_t *, const size_t &);
        template <typename... atypes, std :: enable_if_t <(... && (bytewise :: constraints :: readable <atypes, hasher> ()))> * = nullptr> void update(const atypes & ...);
        hash finalize();
    };
};

#endif
