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
        // Asserts

        static_assert(std :: is_copy_constructible <type> :: value, "Syncset type must be copy constructible.");
        static_assert(bytewise :: constraints :: readable <type, hasher> (), "Syncset type must be hashable (i.e., it must be readable from a hasher).");

    public:

        // Settings

        struct settings
        {
            static constexpr size_t list_threshold = 8;
        };

    private:
    public: // REMOVE ME

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

            prefix(const hash &, const size_t & = 0);
            prefix(const type &, const size_t & = 0);

            // Getters

            size_t bits() const;

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

            node & operator * ();
            node * operator -> ();

            navigator & operator ++ ();
            navigator & operator ++ (int);

            navigator & operator -- ();
            navigator & operator -- (int);

            // Casting

            operator bool () const;
        };

        class multiple
        {
            // Members

            hash _label;
            size_t _size;

            node * _left;
            node * _right;

        public:

            // Constructors

            multiple(const node &, const node &);
            multiple(const multiple &);
            multiple(multiple &&);

            // Destructor

            ~multiple();

            // Getters

            const hash & label() const;
            const size_t & size() const;

            node * left() const;
            node * right() const;

            // Methods

            void refresh();

            bool pullable();
            single pull();

            template <typename vtype> void traverse(vtype &&);
            template <typename vtype> void traverse(vtype &&) const;

            // Operators

            multiple & operator = (const multiple &);
            multiple & operator = (multiple &&);
        };

        class single
        {
            // Members

            hash _label;
            type _element;

        public:

            // Constructors

            single(const type &);

            // Getters

            const hash & label() const;
            const type & element() const;

            // Methods

            empty empty();
            multiple push(const navigation &);

            template <typename vtype> void traverse(vtype &&);
            template <typename vtype> void traverse(vtype &&) const;
        };

        class empty
        {
            // Static members

            static hash _label;

        public:

            // Getters

            const hash & label() const;

            // Methods

            single fill(const type &);

            template <typename vtype> void traverse(vtype &&);
            template <typename vtype> void traverse(vtype &&) const;
        };

        class node : public variant <multiple, single, empty>
        {
        public:

            // Constructors

            node(const multiple &);
            node(const single &);
            node(const empty &);
        };

    public:

        // Nested classes

        class labelset
        {
            // Members

            prefix _prefix;
            hash _label;

        public:

            // Constructors

            labelset() = default;
            labelset(const prefix &, const multiple &);

        public:

            // Getters

            const prefix & prefix() const;
            const hash & label() const;

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
            template <typename vtype> void accept(bytewise :: writer <vtype> &);
        };

        class listset
        {
            // Members

            prefix _prefix;
            type _elements[settings :: list_threshold];
            varint _size;

        public:

            // Constructors

            listset() = default;
            listset(const prefix &, const multiple &);
            listset(const prefix &, const type &);
            listset(const prefix &);

        public:

            // Getters

            const prefix & prefix() const;
            size_t size() const;

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
            template <typename vtype> void accept(bytewise :: writer <vtype> &);

            // Operators

            const type & operator [] (const size_t &) const;
        };

    private:

        // Members

        node _root;

    public:

        // Constructors

        syncset();

        // Methods

        void add(const type &);
        void remove(const type &);

    private:
    public: // REMOVE ME

        // Private methods

        variant <labelset, listset> dump(const prefix &);
    };
};

#endif
