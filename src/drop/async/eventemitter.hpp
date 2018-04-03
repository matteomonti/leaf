#ifndef __drop__async__eventemitter__hpp
#define __drop__async__eventemitter__hpp

// Includes

#include "eventemitter.h"
#include "drop/utils/sfinae.hpp"

namespace drop
{
    // Traits

    template <typename event, typename... types> template <typename lambda> constexpr bool eventemitter <event, types...> :: traits :: is_types_callable()
    {
        return sfinae :: is_valid([](auto && callback) -> decltype(callback(std :: declval <const types &> ()...)) {}).template satisfied <const lambda> ();
    }

    // Constraints

    template <typename event, typename... types> template <typename lambda> constexpr bool eventemitter <event, types...> :: constraints :: callback()
    {
        return traits :: template is_types_callable <lambda> ();
    }

    // callback_interface

    // Destructor

    template <typename event, typename... types> eventemitter <event, types...> :: callback_interface :: ~callback_interface()
    {
    }

    // callback

    // Constructors

    template <typename event, typename... types> template <typename lambda> eventemitter <event, types...> :: callback <lambda> :: callback(const lambda & callback) : _callback(callback)
    {
    }

    // Operators

    template <typename event, typename... types> template <typename lambda> bool eventemitter <event, types...> :: callback <lambda> :: operator () (const types & ... values)
    {
        if constexpr (std :: is_same <decltype(this->_callback(values...)), bool> :: value)
            return this->_callback(values...);
        else
        {
            this->_callback(values...);
            return true;
        }
    }

    // eventemitter

    // Destructor

    template <typename event, typename... types> eventemitter <event, types...> :: ~eventemitter()
    {
        this->clear();
    }

    // Methods

    template <typename event, typename... types> template <typename etype, typename lambda, std :: enable_if_t <(std :: is_same <etype, event> :: value) && (eventemitter <event, types...> :: constraints :: template callback <lambda> ())> *> void eventemitter <event, types...> :: on(const lambda & handler)
    {
        this->_mutex.lock();
        this->_callbacks.push_back(new callback <lambda> (handler));
        this->_mutex.unlock();
    }

    template <typename event, typename... types> void eventemitter <event, types...> :: clear()
    {
        this->_mutex.lock();

        for(size_t i = 0; i < this->_callbacks.size(); i++)
            delete this->_callbacks[i];

        this->_callbacks.clear();

        this->_mutex.unlock();
    }

    // Protected methods

    template <typename event, typename... types> template <typename etype, std :: enable_if_t <std :: is_same <etype, event> :: value> *> bool eventemitter <event, types...> :: emit(const types & ... values)
    {
        this->_mutex.lock();

        for(size_t i = 0; i < this->_callbacks.size(); i++)
            try
            {
                if(!((*(this->_callbacks[i]))(values...)))
                {
                    this->_mutex.unlock();
                    return false;
                }
            }
            catch(...)
            {
                this->_mutex.unlock();
                return false;
            }

        this->_mutex.unlock();
        return true;
    }
};

#endif
