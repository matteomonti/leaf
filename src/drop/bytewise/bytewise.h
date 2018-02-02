// Forward declarations

namespace drop
{
    class bytewise;
};

#if !defined(__forward__) && !defined(__drop__bytewise__bytewise__h)
#define __drop__bytewise__bytewise__h

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

        // Service nested classes

        template <typename atype, typename vtype> struct can_accept
        {
            // SFINAE

        	typedef std :: true_type yes;
        	typedef std :: false_type no;

        	template <typename stype, void (stype :: *) (visitor <vtype> &) const> struct sfinae{};

        	template <class ttype> static yes test(sfinae <ttype, &ttype :: accept> *);
        	template <class ttype> static no test(...);

            // Static members

        	static constexpr bool value = std :: is_same <yes, decltype(test <atype> (nullptr))> :: value;
        };

        template <typename utype> struct can_update
        {
            // SFINAE

        	typedef std :: true_type yes;
        	typedef std :: false_type no;

        	template <typename stype, void (stype :: *) (const uint8_t *, const size_t &)> struct sfinae{};

        	template <class ttype> static yes test(sfinae <ttype, &ttype :: update> *);
        	template <class ttype> static no test(...);

            // Static members

        	static constexpr bool value = std :: is_same <yes, decltype(test <utype> (nullptr))> :: value;
        };

    public:

        // Nested classes

        template <typename vtype> class visitor
        {
            // Asserts

            static_assert(can_update <vtype> :: value, "Visitor must implement an update(const uint8_t *, const size_t) interface.");

            // Members

            vtype & _visitor;

        public:

            // Constructors

            visitor(vtype &);

            // Operators

            template <typename ttype, std :: enable_if_t <std :: is_integral <ttype> :: value> * = nullptr> inline visitor & operator << (const ttype &);
            template <typename atype, std :: enable_if_t <can_accept <atype, vtype> :: value> * = nullptr> inline visitor & operator << (const atype &);
        };
    };
};

#endif
