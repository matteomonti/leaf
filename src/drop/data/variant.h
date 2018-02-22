// Forward declarations

namespace drop
{
    template <typename...> class variant;
};

#if !defined(__forward__) && !defined(__drop__data__variant__h)
#define __drop__data__variant__h

// Includes

#include "drop/bytewise/bytewise.h"
#include "drop/utils/sfinae.h"
#include "drop/utils/static_math.hpp"
#include "drop/utils/enablers.h"

namespace drop
{
    template <typename... types> class variant_base
    {
        // Friends

        template <typename...> friend class variant;

        // Traits

        struct traits
        {
            struct sfinaes
            {
                template <typename ctype, typename vtype> struct call_operator_accepts_reference
                {
                    template <typename ftype, void (ftype :: *)(vtype &)> struct mhelper {};
                    template <typename ftype, void (ftype :: *)(vtype &) const> struct chelper {};

                    template <typename ftype> static std :: false_type sfinae(...);
                    template <typename ftype> static std :: true_type sfinae(mhelper <ftype, &ftype :: operator ()> *);
                    template <typename ftype> static std :: true_type sfinae(chelper <ftype, &ftype :: operator ()> *);

                    static constexpr bool value = std :: is_same <decltype(sfinae <ctype> (0)), std :: true_type> :: value;
                };
            };

            template <typename ctype, typename vtype> static constexpr bool is_callable();
            template <typename ctype, typename vtype> static constexpr bool is_directly_callable();

            template <typename needle, typename haywire, typename... haystack> static constexpr ssize_t typeid_of();
            template <typename vtype, typename... vtypes> static constexpr bool distinct();
        };

        // Constraints

        struct constraints // TODO: Find better names for these constraints
        {
            static constexpr bool variants();
            template <typename vtype> static constexpr bool variant();
            template <typename ctype> static constexpr bool callback();
        };

        // Asserts

        static_assert(constraints :: variants(), "A variant must have one or more distinct types.");

        // Private constructors

        template <typename vtype> variant_base(const vtype &);

        // Members

        uint8_t _typeid;
        std :: aligned_storage_t <max({sizeof(types)...}), max({alignof(types)...})> _value;

        // Private methods

        template <size_t index, typename vtype, typename... vtypes> auto valueptr();
        template <size_t index, typename vtype, typename... vtypes> const auto valueptr() const;
    };

    template <typename... types> class variant : public variant_base <types...>,
                                                 public enablers :: copy_constructible <(... && (std :: is_copy_constructible <types> :: value))>,
                                                 public enablers :: move_constructible <(... && (std :: is_move_constructible <types> :: value))>,
                                                 public enablers :: copy_assignable <(... && (std :: is_copy_assignable <types> :: value))>,
                                                 public enablers :: move_assignable <(... && (std :: is_move_assignable <types> :: value))>
    {
    public:

        // Constraints

        typedef typename variant_base <types...> :: constraints constraints;

    private:

        // Private constructors

        variant();

    public:

        template <typename vtype, std :: enable_if_t <constraints :: template variant <vtype> () && std :: is_copy_constructible <vtype> :: value> * = nullptr> variant(const vtype &);
    };
};

#endif
