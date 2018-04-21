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
#include <memory>

// Includes

#include "promise.hpp"

namespace drop
{
    template <typename... types> class collector
    {
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

            template <typename type> static constexpr bool is_valid();

            template <typename type> static auto declstorage();
            template <typename type> static auto declexception();

            template <typename type> using storage = decltype(declstorage <type> ());
            template <typename type> using exception = decltype(declexception <type> ());

            template <size_t index> using subscript = typename std :: tuple_element <index, std::tuple <types...>> :: type;
        };

        // Constraints

        struct constraints
        {
            static constexpr bool valid();

            template <size_t index> static constexpr bool get_void();
            template <size_t index> static constexpr bool get_element();
            template <size_t index> static constexpr bool get_array();
        };

        // Static asserts

        static_assert(constraints :: valid(), "Template parameters for a collector must be either promises or arrays of promises.");

        // Exceptions

        struct exceptions
        {
            class collection_pending : public std :: exception
            {
                const char * what() const throw();
            };
        };

        // Service nested classes

        struct arc
        {
            // Public members

            bool completed ;

            std :: tuple <typename traits :: template storage <types>...> values;
            std :: tuple <typename traits :: template exception <types>...> exceptions;

            std :: mutex mutex;

            // Constructors

            arc();
        };

        // Members

        std :: shared_ptr <arc> _arc;

    public:

        // Constructors

        collector();

        // Getters

        template <size_t index, std :: enable_if_t <constraints :: template get_void <index> ()> * = nullptr> void get() const;
        template <size_t index, std :: enable_if_t <constraints :: template get_element <index> ()> * = nullptr> const auto & get() const;
        template <size_t index, std :: enable_if_t <constraints :: template get_array <index> ()> * = nullptr> auto get() const;
    };
};

#endif
