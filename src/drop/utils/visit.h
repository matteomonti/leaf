// Forward declarations

namespace drop
{
    class visit;
};

#if !defined(__forward__) && !defined(__drop__utils__visit__h)
#define __drop__utils__visit__h

// Libraries

#include <tuple>
#include <type_traits>

namespace drop
{
    class visit
    {
    public:

        // Static methods

        template <typename... types, typename lambda> static inline void tuple(std :: tuple <types...> &, const lambda &); // TODO: Add SFINAE to accept only callable callbacks
        template <typename... types, typename lambda> static inline void tuple(const std :: tuple <types...> &, const lambda &); // TODO: Add SFINAE to accept only callable callbacks

    private:

        // Private static methods

        template <typename... ttypes, typename lambda, typename... rtypes> static inline void tuple_recursion(std :: tuple <ttypes...> &, const lambda &, rtypes & ...);
        template <typename... ttypes, typename lambda, typename... rtypes> static inline void tuple_recursion(const std :: tuple <ttypes...> &, const lambda &, const rtypes & ...);
    };
};

#endif
