// Forward declarations

namespace drop
{
    class box;
};

#if !defined(__forward__) && !defined(__drop__crypto__box__h)
#define __drop__crypto__box__h

// Libraries

#include <sodium.h>

// Includes

#include "drop/bytewise/bytewise.hpp"

namespace drop
{
    class box
    {
    public:

        // Nested classes

        class publickey
        {
        public:

            // Properties

            static constexpr size_t size = crypto_box_PUBLICKEYBYTES;

        private:

            // Friends

            friend class box;

            // Members

            uint8_t _bytes[size];

        public:

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
            template <typename vtype> void accept(bytewise :: writer <vtype> &);

            // Operators

            const uint8_t & operator [] (const size_t &) const;

            bool operator == (const publickey &) const;

            // Casting

            operator const uint8_t * () const;

        private:

            // Private casting

            operator uint8_t * ();
        };

        class secretkey
        {
        public:

            // Properties

            static constexpr size_t size = crypto_box_SECRETKEYBYTES;

        private:

            // Friends

            friend class box;

            // Members

            uint8_t _bytes[size];

        public:

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
            template <typename vtype> void accept(bytewise :: writer <vtype> &);

            // Operators

            const uint8_t & operator [] (const size_t &) const;

            bool operator == (const secretkey &) const;

            // Casting

            operator const uint8_t * () const;

        private:

            // Private casting

            operator uint8_t * ();
        };

    private:

        // Members

        publickey _publickey;
        secretkey _secretkey;

    public:

        // Constructors

        box();

        // Getters

        const publickey & publickey() const;
        const secretkey & secretkey() const;
    };

    // Ostream integration

    std :: ostream & operator << (std :: ostream &, const class box :: publickey &);
    std :: ostream & operator << (std :: ostream &, const class box :: secretkey &);
};

#endif
