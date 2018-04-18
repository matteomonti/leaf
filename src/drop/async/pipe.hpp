#ifndef __drop__async__pipe__hpp
#define __drop__async__pipe__hpp

// Includes

#include "pipe.h"

namespace drop
{
    // Methods

    template <typename type> void pipe <type> :: push(const type & element)
    {
        optional <promise <type>> promise;

        this->_mutex.lock();

        if(this->_promise)
        {
            promise = this->_promise;
            this->_promise = null;
        }
        else
            this->_pipe.push_back(element);

        this->_mutex.unlock();

        if(promise)
            promise->resolve(element);
    }

    template <typename type> promise <type> pipe <type> :: pop()
    {
        promise <type> promise;

        this->_mutex.lock();

        if(this->_pipe.size())
        {
            promise.resolve(this->_pipe.front());
            this->_pipe.pop_front();
        }
        else
            this->_promise = promise;

        this->_mutex.unlock();

        return promise;
    }
};

#endif
