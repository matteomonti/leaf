// Forward declarations

namespace drop
{
    class bytewise;
};

#if !defined(__forward__) && !defined(__drop__bytewise__bytewise__h)
#define __drop__bytewise__bytewise__h

// Libraries

#include <type_traits>
#include <vector>

// Includes

#include "endianess.h"

namespace drop
{
    class bytewise
    {
    public:

        // Forward declarations

        template <typename vtype> class visitor;

    private:

        // Traits

        struct traits
        {
            template <typename atype, typename vtype> struct can_accept
            {
            	template <typename stype, void (stype :: *) (visitor <vtype> &) const> struct sfinae{};

            	template <typename ttype> static std :: true_type test(sfinae <ttype, &ttype :: accept> *);
            	template <typename ttype> static std :: false_type test(...);

            	static constexpr bool value = std :: is_same <std :: true_type, decltype(test <atype> (nullptr))> :: value;
            };

            template <typename utype> struct can_update
            {
            	template <typename stype, void (stype :: *) (const uint8_t *, const size_t &)> struct sfinae{};

            	template <typename ttype> static std :: true_type test(sfinae <ttype, &ttype :: update> *);
            	template <typename ttype> static std :: false_type test(...);

            	static constexpr bool value = std :: is_same <std :: true_type, decltype(test <utype> (nullptr))> :: value;
            };

            template <typename vtype> struct is_vector;
            template <typename itype> struct is_vector <std :: vector <itype>>
            {
                static constexpr bool value = true;
                typedef itype type;
            };

            template <typename vtype> struct is_vector
            {
                static constexpr bool value = false;
            };
        };

        // Constraints

        struct constraints
        {
            template <typename atype, typename vtype> static constexpr bool acceptor();
            template <typename vtype> static constexpr bool visitor();
        };

    public:

        // Nested classes

        template <typename vtype> class visitor
        {
            // Asserts

            static_assert(constraints :: visitor <vtype> (), "Visitor must implement an update(const uint8_t *, const size_t) interface.");

            // Members

            vtype & _visitor;

        public:

            // Constructors

            visitor(vtype &);

            // Operators

            template <typename atype, std :: enable_if_t <constraints :: acceptor <atype, vtype> ()> * = nullptr> inline visitor & operator << (const atype &);
        };

        // Static methods

        template <typename vtype, typename... atypes, std :: enable_if_t <constraints :: visitor <vtype> () && (... && (constraints :: acceptor <atypes, vtype> ()))> * = nullptr> static inline void visit(vtype &, const atypes & ...);
    };
};

#endif
