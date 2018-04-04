// Forward declarations

namespace drop
{
    template <typename> class optional;
    class def;
    class null;
};

#if !defined(__forward__) && !defined(__drop__data__optional__h)
#define __drop__data__optional__h

// Libraries

#include <memory>

// Includes

#include "drop/utils/enablers.h"

namespace drop
{
    template <typename type> class optional_base
    {
        // Members

        bool _set;
        std :: aligned_storage_t <sizeof(type), alignof(type)> _value;

    public:

        // Constructors

        optional_base();
        optional_base(null);
        optional_base(def);
        template <typename... types, std :: enable_if_t <std :: is_constructible <type, types...> :: value> * = nullptr> optional_base(types && ...);

        optional_base(const optional_base &);
        optional_base(optional_base &&);

        // Destructor

        ~optional_base();

        // Methods

        template <typename... types, std :: enable_if_t <std :: is_constructible <type, types...> :: value> * = nullptr> void emplace(types && ...);

        // Operators

        type & operator * ();
        const type & operator * () const;

        type * operator -> ();
        const type * operator -> () const;

        optional_base & operator = (null);

        optional_base & operator = (const type &);
        optional_base & operator = (type &&);

        optional_base & operator = (const optional_base &);
        optional_base & operator = (optional_base &&);

        // Casting

        operator const bool & () const;
    };

    template <typename type> class optional : public optional_base <type>,
                                              public enablers :: copy_constructible <std :: is_copy_constructible <type> :: value>,
                                              public enablers :: move_constructible <std :: is_move_constructible <type> :: value>,
                                              public enablers :: copy_assignable <std :: is_copy_assignable <type> :: value && std :: is_copy_constructible <type> :: value>,
                                              public enablers :: move_assignable <std :: is_move_assignable <type> :: value && std :: is_copy_constructible <type> :: value>
    {
    public:

        // Constructors

        optional() = default;
        template <typename dtype, std :: enable_if_t <std :: is_same <dtype, def> :: value && std :: is_default_constructible <type> :: value> * = nullptr> optional(dtype);
        optional(null);
        template <typename... types, std :: enable_if_t <std :: is_constructible <type, types...> :: value> * = nullptr> optional(types && ...);

        // Operators

        optional & operator = (null);

        template <typename ttype, std :: enable_if_t <std :: is_same <ttype, type> :: value && std :: is_copy_assignable <type> :: value && std :: is_copy_constructible <type> :: value> * = nullptr> optional & operator = (const ttype &);
        template <typename ttype, std :: enable_if_t <std :: is_same <ttype, type> :: value && std :: is_move_assignable <type> :: value && std :: is_move_constructible <type> :: value> * = nullptr> optional & operator = (ttype &&);
    };

    class def
    {
    };

    class null
    {
    };

    extern def def;
    extern null null;
};

#endif
