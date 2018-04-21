#ifndef __drop__async__collector__hpp
#define __drop__async__collector__hpp

// Includes

#include "collector.h"

namespace drop
{
    // Traits

    template <typename... types> template <typename type> constexpr bool collector <types...> :: traits :: is_valid()
    {
        typedef unmark <type> utype;

        if constexpr (is_promise <utype> :: value)
            return true;
        else if constexpr(is_array <utype> :: value)
        {
            typedef typename is_array <utype> :: type ptype;
            return is_promise <ptype> :: value;
        }
        else
            return false;
    }

    template <typename... types> template <typename type> auto collector <types...> :: traits :: declremoverequired()
    {
        if constexpr (is_required <type> :: value)
            return std :: declval <typename is_required <type> :: type> ();
        else
            return std :: declval <type> ();
    }

    template <typename... types> template <typename type> auto collector <types...> :: traits :: declremoveuntil()
    {
        if constexpr (is_until <type> :: value)
            return std :: declval <typename is_until <type> :: type> ();
        else
            return std :: declval <type> ();
    }

    template <typename... types> template <typename type> auto collector <types...> :: traits :: declunmark()
    {
        return declremoveuntil <decltype(declremoverequired <decltype(declremoveuntil <type> ())> ())> ();
    }

    template <typename... types> template <typename type> auto collector <types...> :: traits :: declstorage()
    {
        typedef unmark <type> utype;

        if constexpr (is_array <utype> :: value)
        {
            typedef typename is_array <utype> :: type ptype;
            static constexpr size_t size = is_array <utype> :: size;

            if constexpr (std :: is_same <ptype, promise <void>> :: value)
                return std :: declval <std :: array <bool, size>> ();
            else
            {
                typedef typename is_promise <ptype> :: type ttype;
                return std :: declval <std :: array <optional <ttype>, size>> ();
            }
        }
        else
        {
            typedef typename is_promise <utype> :: type ttype;
            if constexpr (std :: is_same <ttype, void> :: value)
                return std :: declval <bool> ();
            else
                return std :: declval <optional <ttype>> ();
        }
    }

    template <typename... types> template <typename type> auto collector <types...> :: traits :: declexception()
    {
        typedef unmark <type> utype;

        if constexpr (is_array <utype> :: value)
        {
            static constexpr size_t size = is_array <utype> :: size;
            return std :: declval <std :: array <std :: exception_ptr, size>> ();
        }
        else
            return std :: declval <std :: exception_ptr> ();
    }

    template <typename... types> template <typename type> constexpr bool collector <types...> :: traits :: has_required()
    {
        return is_required <type> :: value || is_required <decltype(declremoveuntil <type> ())> :: value;
    }

    template <typename... types> template <typename type> constexpr bool collector <types...> :: traits :: has_until()
    {
        return is_until <type> :: value || is_until <decltype(declremoverequired <type> ())> :: value;
    }

    template <typename... types> template <size_t index> constexpr bool collector <types...> :: traits :: index_has_required()
    {
        return has_required <subscript <index>> ();
    }

    template <typename... types> template <size_t index> constexpr bool collector <types...> :: traits :: index_has_until()
    {
        return has_until <subscript <index>> ();
    }

    template <typename... types> constexpr size_t collector <types...> :: traits :: required_count()
    {
        return (... + (size_t(has_required <types> ())));
    }

    template <typename... types> constexpr size_t collector <types...> :: traits :: until_count()
    {
        return (... + (size_t(has_until <types> ())));
    }

    // Constraints

    template <typename... types> constexpr bool collector <types...> :: constraints :: valid()
    {
        return (sizeof...(types) > 0) && (... && (traits :: template is_valid <types> ()));
    }

    template <typename... types> template <size_t index> constexpr bool collector <types...> :: constraints :: get_void()
    {
        if constexpr (index < sizeof...(types))
            return std :: is_same <typename traits :: template unmark <typename traits :: template subscript <index>>, promise <void>> :: value;
        else
            return false;
    }

    template <typename... types> template <size_t index> constexpr bool collector <types...> :: constraints :: get_element()
    {
        if constexpr (index < sizeof...(types))
            return !(std :: is_same <typename traits :: template unmark <typename traits :: template subscript <index>>, promise <void>> :: value) && !(traits :: template is_array <typename traits :: template subscript <index>> :: value);
        else
            return false;
    }

    template <typename... types> template <size_t index> constexpr bool collector <types...> :: constraints :: get_void_array()
    {
        if constexpr (index < sizeof...(types))
        {
            typedef typename traits :: template unmark <typename traits :: template subscript <index>> type;
            if constexpr (traits :: template is_array <type> :: value)
            {
                typedef typename traits :: template is_array <type> :: type ptype;
                return std :: is_same <ptype, promise <void>> :: value;
            }
            else
                return false;
        }
        else
            return false;
    }

    template <typename... types> template <size_t index> constexpr bool collector <types...> :: constraints :: get_element_array()
    {
        if constexpr (index < sizeof...(types))
        {
            typedef typename traits :: template unmark <typename traits :: template subscript <index>> type;
            if constexpr (traits :: template is_array <type> :: value)
            {
                typedef typename traits :: template is_array <type> :: type ptype;
                return !(std :: is_same <ptype, promise <void>> :: value);
            }
            else
                return false;
        }
        else
            return false;
    }

    // Exceptions

    template <typename... types> const char * collector <types...> :: exceptions :: collection_pending :: what() const throw()
    {
        return "Collection pending.";
    }

    // arc

    // Constructors

    template <typename... types> collector <types...> :: arc :: arc() : completed(false)
    {
    }

    // collector

    // Constructors

    template <typename... types> collector <types...> :: collector() : _arc(std :: make_shared <arc> ())
    {
    }

    // Getters

    template <typename... types> template <size_t index, std :: enable_if_t <collector <types...> :: constraints :: template get_void <index> ()> *> void collector <types...> :: get() const
    {
        this->_arc->mutex.lock();

        if(std :: exception_ptr exception = std :: get <index> (this->_arc->exceptions))
        {
            this->_arc->mutex.unlock();
            std :: rethrow_exception(exception);
        }
        else if(!(std :: get <index> (this->_arc->values)))
        {
            this->_arc->mutex.unlock();
            throw (class exceptions :: collection_pending){};
        }
        else
            this->_arc->mutex.unlock();
    }

    template <typename... types> template <size_t index, std :: enable_if_t <collector <types...> :: constraints :: template get_element <index> ()> *> const auto & collector <types...> :: get() const
    {
        this->_arc->mutex.lock();

        if(std :: exception_ptr exception = std :: get <index> (this->_arc->exceptions))
        {
            this->_arc->mutex.unlock();
            std :: rethrow_exception(exception);
        }
        else if(!(std :: get <index> (this->_arc->values)))
        {
            this->_arc->mutex.unlock();
            throw (class exceptions :: collection_pending){};
        }
        else
        {
            const auto & value = *(std :: get <index> (this->_arc->values));
            this->_arc->mutex.unlock();
            return value;
        }
    }

    template <typename... types> template <size_t index, std :: enable_if_t <collector <types...> :: constraints :: template get_void_array <index> ()> *> void collector <types...> :: get(const size_t & arrindex) const
    {
        this->_arc->mutex.lock();

        if(std :: exception_ptr exception = std :: get <index> (this->_arc->exceptions)[arrindex])
        {
            this->_arc->mutex.unlock();
            std :: rethrow_exception(exception);
        }
        else if(!(std :: get <index> (this->_arc->values)[arrindex]))
        {
            this->_arc->mutex.unlock();
            throw (class exceptions :: collection_pending){};
        }
        else
            this->_arc->mutex.unlock();
    }

    template <typename... types> template <size_t index, std :: enable_if_t <collector <types...> :: constraints :: template get_element_array <index> ()> *> const auto & collector <types...> :: get(const size_t & arrindex) const
    {
        this->_arc->mutex.lock();

        if(std :: exception_ptr exception = std :: get <index> (this->_arc->exceptions)[arrindex])
        {
            this->_arc->mutex.unlock();
            std :: rethrow_exception(exception);
        }
        else if(!(std :: get <index> (this->_arc->values)[arrindex]))
        {
            this->_arc->mutex.unlock();
            throw (class exceptions :: collection_pending){};
        }
        else
        {
            const auto & value = *(std :: get <index> (this->_arc->values)[arrindex]);
            this->_arc->mutex.unlock();
            return value;
        }
    }
};

#endif
