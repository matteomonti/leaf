// Forward declarations

namespace drop
{
    template <typename...> class collector;
};

#if !defined(__forward__) && !defined(__drop__async__collector__h)
#define __drop__async__collector__h

// Libraries

#include <type_traits>
#include <array>
#include <exception>

// Includes

#include "promise.hpp"

namespace drop
{
    template <typename... types> class collector
    {
    public: // REMOVE ME

        // Traits

        struct traits
        {
            template <typename> struct is_promise;

            template <typename ttype> struct is_promise <promise <ttype>>
            {
                static constexpr bool value = true;
                typedef ttype type;
            };

            template <typename ttype> struct is_promise
            {
                static constexpr bool value = false;
            };

            template <typename> struct is_array;

            template <typename ttype, size_t ssize> struct is_array <std :: array <ttype, ssize>>
            {
                static constexpr bool value = true;

                typedef ttype type;
                static constexpr size_t size = ssize;
            };

            template <typename ttype> struct is_array
            {
                static constexpr bool value = false;
            };

            template <typename type> static constexpr bool valid();

            template <typename type> static auto declstorage();
            template <typename type> static auto declexception();

            template <typename type> using storage = decltype(declstorage <type> ());
            template <typename type> using exception = decltype(declexception <type> ());
        };
    };
};

#endif
