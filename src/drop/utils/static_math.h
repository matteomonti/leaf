#if !defined(__forward__) && !defined(__drop__utils__static_math__h)
#define __drop__utils__static_math__h

namespace drop
{
    template <typename type, size_t size> constexpr type max(const type (&) [size]);
};

#endif
