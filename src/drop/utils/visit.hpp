#ifndef __drop__utils__visit__hpp
#define __drop__utils__visit__hpp

// Includes

#include "visit.h"

namespace drop
{
    // Static methods

    template <typename... types, typename lambda> inline void visit :: tuple(std :: tuple <types...> & tuple, const lambda & callback)
    {
        tuple_recursion(tuple, callback);
    }

    template <typename... types, typename lambda> inline void visit :: tuple(const std :: tuple <types...> & tuple, const lambda & callback)
    {
        tuple_recursion(tuple, callback);
    }

    // Private static methods

    template <typename... ttypes, typename lambda, typename... rtypes> inline void visit :: tuple_recursion(std :: tuple <ttypes...> & tuple, const lambda & callback, rtypes & ... items)
    {
        if constexpr (sizeof...(ttypes) == sizeof...(rtypes))
            callback(items...);
        else
            tuple_recursion(tuple, callback, items..., std :: get <sizeof...(rtypes)> (tuple));
    }

    template <typename... ttypes, typename lambda, typename... rtypes> inline void visit :: tuple_recursion(const std :: tuple <ttypes...> & tuple, const lambda & callback, const rtypes & ... items)
    {
        if constexpr (sizeof...(ttypes) == sizeof...(rtypes))
            callback(items...);
        else
            tuple_recursion(tuple, callback, items..., std :: get <sizeof...(rtypes)> (tuple));
    }
};

#endif
