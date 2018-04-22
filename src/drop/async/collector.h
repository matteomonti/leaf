// Forward declarations

namespace drop
{
    template <typename...> class collector;

    namespace markers
    {
        template <typename> struct required;
        template <typename> struct until;
    };
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
            template <typename> struct is_required;

            template <typename ttype> struct is_required <markers :: required <ttype>>
            {
                static constexpr bool value = true;
                typedef ttype type;
            };

            template <typename ttype> struct is_required
            {
                static constexpr bool value = false;
            };

            template <typename> struct is_until;

            template <typename ttype> struct is_until <markers :: until <ttype>>
            {
                static constexpr bool value = true;
                typedef ttype type;
            };

            template <typename ttype> struct is_until
            {
                static constexpr bool value = false;
            };

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

            template <typename type> static auto declremoverequired();
            template <typename type> static auto declremoveuntil();
            template <typename type> static auto declunmark();

            template <typename type> static auto declstorage();
            template <typename type> static auto declexception();

            template <typename type> using unmark = decltype(declunmark <type> ());
            template <typename type> using storage = decltype(declstorage <type> ());
            template <typename type> using exception = decltype(declexception <type> ());

            template <size_t index> using subscript = typename std :: tuple_element <index, std::tuple <types...>> :: type;

            template <typename type> static constexpr bool has_required();
            template <typename type> static constexpr bool has_until();

            template <size_t index> static constexpr bool index_has_required();
            template <size_t index> static constexpr bool index_has_until();

            template <size_t index = 0> static constexpr size_t total_promises();
            template <size_t index = 0> static constexpr size_t required_promises();
        };

    public:

        // Constraints

        struct constraints
        {
            template <typename... ptypes> static constexpr bool valid();

            template <size_t index> static constexpr bool get_void();
            template <size_t index> static constexpr bool get_element();
            template <size_t index> static constexpr bool get_void_array();
            template <size_t index> static constexpr bool get_element_array();
        };

        // Static asserts

        static_assert(constraints :: template valid <types...> (), "Template parameters for a collector must be either promises or arrays of promises.");

        // Exceptions

        struct exceptions
        {
            class collection_pending : public std :: exception
            {
                const char * what() const throw();
            };

            class required_missing : public std :: exception
            {
                const char * what() const throw();
            };
        };

    private:

        // Service nested classes

        struct arc
        {
            // Public members

            optional <promise <collector <types...>>> promise;

            size_t total;
            size_t required;

            std :: tuple <typename traits :: template storage <types>...> values;
            std :: tuple <typename traits :: template exception <types>...> exceptions;

            std :: recursive_mutex mutex;

            // Constructors

            arc(const drop :: promise <collector <types...>> &);
        };

        // Members

        std :: shared_ptr <arc> _arc;

    public:

        // Constructors

        collector(const promise <collector <types...>> &, const typename traits :: template unmark <types> & ...);

        // Getters

        template <size_t index, std :: enable_if_t <constraints :: template get_void <index> ()> * = nullptr> void get() const;
        template <size_t index, std :: enable_if_t <constraints :: template get_element <index> ()> * = nullptr> const auto & get() const;
        template <size_t index, std :: enable_if_t <constraints :: template get_void_array <index> ()> * = nullptr> void get(const size_t &) const;
        template <size_t index, std :: enable_if_t <constraints :: template get_element_array <index> ()> * = nullptr> const auto & get(const size_t &) const;

    private:

        // Private methods

        template <size_t index = 0, typename ptype, typename... ptypes> void handle(const ptype &, const ptypes & ...) const;

        template <size_t> void resolve() const;
        template <size_t> void reject(const std :: exception_ptr &) const;
    };

    // Functions

    template <typename... types> promise <collector <types...>> collect(const types & ...);

    // Markers

    namespace markers
    {
        template <typename type> struct required : public type
        {
            required(const type &);
        };

        template <typename type> struct until : public type
        {
            until(const type &);
        };
    };

    template <typename type> auto required(const type &);
    template <typename type> auto until(const type &);
};

#endif
