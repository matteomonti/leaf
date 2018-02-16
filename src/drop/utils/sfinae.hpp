#ifndef __drop__utils__sfinae__hpp
#define __drop__utils__sfinae__hpp

// Includes

#include "sfinae.h"

namespace drop
{
    // evaluator

    // Methods

    template <typename expression, typename rettype> template <typename type> constexpr bool sfinae :: evaluator <expression, rettype> :: satisfied() const
    {
        if constexpr (std :: is_same <decltype(test <type> (int())), std :: true_type> :: value)
        {
            if constexpr (std :: is_same <rettype, any> :: value)
                return true;

            return std :: is_same <decltype(std :: declval <expression> ()(std :: declval <type> ())), rettype> :: value;
        }
        else
            return false;
    }

    // Private methods

    template <typename expression, typename rettype> template <typename type> constexpr auto sfinae :: evaluator <expression, rettype> :: test(int) const -> decltype(std :: declval <expression> ()(std :: declval <type> ()), std :: true_type())
    {
        return std :: true_type();
    }

    template <typename expression, typename rettype> template <typename type> constexpr std :: false_type sfinae :: evaluator <expression, rettype> :: test(...) const
    {
        return std :: false_type();
    }

    // sfinae

    // Static methods

    template <typename expression> constexpr auto sfinae :: is_valid(const expression &)
    {
        return evaluator <expression, any> ();
    }

    template <typename rettype, typename expression> constexpr auto sfinae :: returns(const expression &)
    {
        return evaluator <expression, rettype> ();
    }
};

#endif
