#ifndef __drop__network__pool__hpp
#define __drop__network__pool__hpp

// Includes

#include "pool.h"
#include "drop/async/promise.hpp"
#include "drop/thread/channel.hpp"

namespace drop
{
    // connection

    // Methods

    template <typename... types, std :: enable_if_t <(... && (bytewise :: constraints :: serializable <types> ()))> *> promise <void> pool :: connection :: send(const types & ... messages) const
    {
        return this->send(bytewise :: serialize(messages...));
    }

    template <typename... types, std :: enable_if_t <(... && (bytewise :: constraints :: deserializable <types> ()))> *> auto pool :: connection :: receive() const
    {
        promise <decltype(bytewise :: deserialize <types...> (std :: declval <const buffer &> ()))> promise;

        this->receive().then([=](const buffer & message)
        {
            promise.resolve(bytewise :: deserialize <types...> (message));
        }).except([=](const std :: exception_ptr & exception)
        {
            promise.reject(exception);
        });

        return promise;
    }
};

#endif
