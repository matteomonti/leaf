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

        struct constraints
        {
            static constexpr bool valid_types();

            template <typename vtype> static constexpr bool variant();

            template <typename vtype> static constexpr bool mutable_visitor();
            template <typename vtype> static constexpr bool const_visitor();

            template <typename ctype> static constexpr bool mutable_case();
            template <typename ctype> static constexpr bool const_case();
        };

        // Asserts

        static_assert(constraints :: valid_types(), "A variant must have one or more distinct types.");

        // Private constructors

        variant_base();
        template <typename vtype> variant_base(const vtype &);

        // Members

        uint8_t _typeid;
        std :: aligned_storage_t <max({sizeof(types)...}), max({alignof(types)...})> _value;

    public:

        // Methods

        template <typename lambda, std :: enable_if_t <constraints :: template mutable_visitor <lambda> ()> * = nullptr> void visit(lambda &&);
        template <typename lambda, std :: enable_if_t <constraints :: template const_visitor <lambda> ()> * = nullptr> void visit(lambda &&) const;

        template <typename... lambdas, std :: enable_if_t <(... && (constraints :: template mutable_case <lambdas> ()))> * = nullptr> void match(lambdas && ...);
        template <typename... lambdas, std :: enable_if_t <(... && (constraints :: template mutable_case <lambdas> ()))> * = nullptr> void match(lambdas && ...) const;

    private:

        // Private methods

        template <typename vtype, typename... vtypes, typename lambda> void unwrap(const size_t &, lambda &&);
        template <typename vtype, typename... vtypes, typename lambda> void unwrap(const size_t &, lambda &&) const;

        template <typename vtype, typename lambda, typename... lambdas> void direct_dispatch(vtype &, lambda &&, lambdas && ...);
        template <typename vtype, typename lambda, typename... lambdas> void direct_dispatch(const vtype &, lambda &&, lambdas && ...) const;

        template <typename vtype, typename lambda, typename... lambdas> void dispatch(vtype &, lambda &&, lambdas && ...);
        template <typename vtype, typename lambda, typename... lambdas> void dispatch(const vtype &, lambda &&, lambdas && ...) const;
    };

    template <typename... types> class variant : public variant_base <types...>,
                                                 public enablers :: copy_constructible <(... && (std :: is_copy_constructible <types> :: value))>,
                                                 public enablers :: move_constructible <(... && (std :: is_move_constructible <types> :: value))>,
                                                 public enablers :: copy_assignable <(... && (std :: is_copy_assignable <types> :: value))>,
                                                 public enablers :: move_assignable <(... && (std :: is_move_assignable <types> :: value))>
    {
        // Traits

        typedef typename variant_base <types...> :: traits traits;

    public:

        // Constraints

        typedef typename variant_base <types...> :: constraints constraints;

    private:

        // Private constructors

        variant();

    public:

        // Constructors

        template <typename vtype, std :: enable_if_t <constraints :: template variant <vtype> () && std :: is_copy_constructible <vtype> :: value> * = nullptr> variant(const vtype &);

        // Static methods

        template <typename vtype, typename... atypes, std :: enable_if_t <constraints :: template variant <vtype> () && std :: is_constructible <vtype, atypes...> :: value> * = nullptr> static variant construct(atypes && ...);
    };
};

#endif