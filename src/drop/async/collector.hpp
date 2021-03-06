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

    template <typename... types> template <size_t index> constexpr size_t collector <types...> :: traits :: total_promises()
    {
        if constexpr (index < sizeof...(types))
        {
            typedef unmark <subscript <index>> type;

            if constexpr (is_array <type> :: value)
                return is_array <type> :: size + total_promises <index + 1> ();
            else
                return 1 + total_promises <index + 1> ();
        }
        else
            return 0;
    }

    template <typename... types> template <size_t index> constexpr size_t collector <types...> :: traits :: required_promises()
    {
        if constexpr (index < sizeof...(types))
        {
            if constexpr (index_has_required <index> ())
            {
                typedef unmark <subscript <index>> type;

                if constexpr (is_array <type> :: value)
                    return is_array <type> :: size + required_promises <index + 1> ();
                else
                    return 1 + required_promises <index + 1> ();
            }
            else
                return required_promises <index + 1> ();
        }
        else
            return 0;
    }

    // Constraints

    template <typename... types> template <typename... ptypes> constexpr bool collector <types...> :: constraints :: valid()
    {
        return (sizeof...(ptypes) > 0) && (... && (traits :: template is_valid <ptypes> ()));
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

    template <typename... types> const char * collector <types...> :: exceptions :: required_missing :: what() const throw()
    {
        return "Until-marked promise was resolved before all requried promises were resolved.";
    }

    // arc

    // Constructors

    template <typename... types> collector <types...> :: arc :: arc(const drop :: promise <collector <types...>> & promise) : promise(promise), total(0), required(0)
    {
    }

    // collector

    // Constructors

    template <typename... types> collector <types...> :: collector(const promise <collector <types...>> & promise, const typename traits :: template unmark <types> & ... promises) : _arc(std :: make_shared <arc> (promise))
    {
        this->handle(promises...);
    }

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

    // Private methods

    template <typename... types> template <size_t index, typename ptype, typename... ptypes> void collector <types...> :: handle(const ptype & head, const ptypes & ... tail) const
    {
        if constexpr (traits :: template is_array <ptype> :: value)
        {
            for(size_t i = 0; i < traits :: template is_array <ptype> :: size; i++)
            {
                typedef typename traits :: template is_array <ptype> :: type rtype;

                if constexpr(std :: is_same <rtype, promise <void>> :: value)
                    head[i].then([*this, i]()
                    {
                        this->_arc->mutex.lock();
                        if(this->_arc->promise)
                        {
                            std :: get <index> (this->_arc->values)[i] = true;
                            this->resolve <index> ();
                        }
                        this->_arc->mutex.unlock();
                    }).except([*this, i](const std :: exception_ptr & exception)
                    {
                        this->_arc->mutex.lock();
                        if(this->_arc->promise)
                        {
                            std :: get <index> (this->_arc->exceptions)[i] = exception;
                            this->reject <index> (exception);
                        }
                        this->_arc->mutex.unlock();
                    });
                else
                    head[i].then([*this, i](const auto & value)
                    {
                        this->_arc->mutex.lock();
                        if(this->_arc->promise)
                        {
                            std :: get <index> (this->_arc->values)[i] = value;
                            this->resolve <index> ();
                        }
                        this->_arc->mutex.unlock();
                    }).except([*this, i](const std :: exception_ptr & exception)
                    {
                        this->_arc->mutex.lock();
                        if(this->_arc->promise)
                        {
                            std :: get <index> (this->_arc->exceptions)[i] = exception;
                            this->reject <index> (exception);
                        }
                        this->_arc->mutex.unlock();
                    });
            }
        }
        else
        {
            if constexpr (std :: is_same <ptype, promise <void>> :: value)
                head.then([*this]()
                {
                    this->_arc->mutex.lock();
                    if(this->_arc->promise)
                    {
                        std :: get <index> (this->_arc->values) = true;
                        this->resolve <index> ();
                    }
                    this->_arc->mutex.unlock();
                }).except([*this](const std :: exception_ptr & exception)
                {
                    this->_arc->mutex.lock();
                    if(this->_arc->promise)
                    {
                        std :: get <index> (this->_arc->exceptions) = exception;
                        this->reject <index> (exception);
                    }
                    this->_arc->mutex.unlock();
                });
            else
                head.then([*this](const auto & value)
                {
                    this->_arc->mutex.lock();
                    if(this->_arc->promise)
                    {
                        std :: get <index> (this->_arc->values) = value;
                        this->resolve <index> ();
                    }
                    this->_arc->mutex.unlock();
                }).except([*this](const std :: exception_ptr & exception)
                {
                    this->_arc->mutex.lock();
                    if(this->_arc->promise)
                    {
                        std :: get <index> (this->_arc->exceptions) = exception;
                        this->reject <index> (exception);
                    }
                    this->_arc->mutex.unlock();
                });
        }

        if constexpr (sizeof...(ptypes) > 0)
            this->handle <index + 1> (tail...);
    }

    template <typename... types> template <size_t index> void collector <types...> :: resolve() const
    {
        this->_arc->total++;
        if constexpr (traits :: template index_has_required <index> ())
            this->_arc->required++;

        if constexpr (traits :: template index_has_until <index> ())
        {
            if(this->_arc->required == traits :: required_promises())
            {
                this->_arc->promise->resolve(*this);
                this->_arc->promise = null;
            }
            else
            {
                this->_arc->promise->reject((class exceptions :: required_missing){});
                this->_arc->promise = null;
            }
        }
        else
        {
            if(this->_arc->total == traits :: total_promises())
            {
                this->_arc->promise->resolve(*this);
                this->_arc->promise = null;
            }
        }
    }

    template <typename... types> template <size_t index> void collector <types...> :: reject(const std :: exception_ptr & exception) const
    {
        this->_arc->total++;
        if constexpr (traits :: template index_has_required <index> ())
            this->_arc->required++;

        if constexpr (traits :: template index_has_required <index> ())
        {
            this->_arc->promise->reject(exception);
            this->_arc->promise = null;
        }
        else
        {
            if(this->_arc->total == traits :: total_promises())
            {
                this->_arc->promise->resolve(*this);
                this->_arc->promise = null;
            }
        }
    }

    // Functions

    template <typename... types> promise <collector <types...>> collect(const types & ... promises)
    {
        promise <collector <types...>> promise;
        collector <types...> collector(promise, promises...);
        return promise;
    }

    // Markers

    namespace markers
    {
        template <typename type> required <type> :: required(const type & _) : type(_)
        {
        }

        template <typename type> until <type> :: until(const type & _) : type(_)
        {
        }
    };

    template <typename type> auto required(const type & _)
    {
        return markers :: required <type> (_);
    }

    template <typename type> auto until(const type & _)
    {
        return markers :: until <type> (_);
    }
};

#endif
