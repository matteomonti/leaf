// Forward declarations

namespace drop
{
    template <typename> class syncset;
};

#if !defined(__forward__) && !defined(__drop__data__syncset__h)
#define __drop__data__syncset__h

// Includes

#include "drop/bytewise/bytewise.h"
#include "drop/crypto/hash.h"
#include "variant.h"
#include "varint.h"

namespace drop
{
    template <typename type> class syncset
    {
    public: // REMOVE ME

        // Asserts

        static_assert(std :: is_copy_constructible <type> :: value, "Syncset type must be copy constructible.");
        static_assert(bytewise :: constraints :: readable <type, hasher> (), "Syncset type must be hashable (i.e., it must be readable from a hasher).");

        // Service nested enums

        enum navigation {left, right};

        // Forward declarations

        class prefix;
        class navigator;

        class node;
        class multiple;
        class single;
        class empty;

        // Service nested classes

        class prefix
        {
            // Members

            uint8_t _value[hash :: size];
            varint _bits;

        public:

            // Constructors

            prefix(const type &, const size_t & = 0);

            // Getters

            const size_t & bits() const;

            // Setters

            void bits(const size_t &);

            // Methods

            template <typename vtype> void visit(bytewise :: reader <vtype> &) const;
            template <typename vtype> void visit(bytewise :: writer <vtype> &);

            // Operators

            bool operator [] (const size_t &) const;
        };

        class navigator
        {
            // Members

            prefix _prefix;
            size_t _depth;
            node * _trace[8 * hash :: size];

        public:

            // Constructors

            navigator(const prefix &, node &);

            // Getters

            const size_t & depth() const;
            navigation next() const;

            // Operators

            node * operator -> ();

            navigator & operator ++ ();
            navigator & operator ++ (int);

            navigator & operator -- ();
            navigator & operator -- (int);
        };

        class multiple
        {
            // Members

            hash _label;
            node * _left;
            node * _right;

        public:

            // Constructors

            multiple(const node &, const node &);

            // Destructor

            ~multiple();

            // Methods

            void relabel();

            bool pullable();
            single pull();
        };

        class single
        {
            // Members

            hash _label;
            type _element;

        public:

            // Constructors

            single(const type &);

            // Methods

            empty empty();
            multiple push(const navigation &);
        };

        class empty
        {
            // Static members

            static hash _label;

        public:

            // Methods

            single fill(const type &);
        };

        class node : public variant <multiple, single, empty>
        {
        public:

            // Constructors

            node(const multiple &);
            node(const single &);
            node(const empty &);

            // Methods

            void relabel();
        };

        // Members

        node _root;

    public:

        // Constructors

        syncset();
    };
};

#endif
