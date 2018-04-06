#ifndef __vine__network__multiplexer__hpp
#define __vine__network__multiplexer__hpp

// Includes

#include "multiplexer.h"

namespace vine
{
    // Traits

    template <typename dialer, uint8_t channels> template <typename... types> constexpr bool multiplexer <dialer, channels> :: traits :: is_leech_constructible()
    {
        return std :: is_constructible <dialer, types...> :: value && (... || (std :: is_same <types, pool &> :: value));
    }

    template <typename dialer, uint8_t channels> template <typename type, typename... types> constexpr bool multiplexer <dialer, channels> :: traits :: is_rotate_constructible()
    {
        if constexpr (std :: is_same <type, pool &> :: value)
            return std :: is_constructible <dialer, types...> :: value;
        else if constexpr (!(... || (std :: is_same <types, pool &> :: value)))
            return false;
        else
            return is_rotate_constructible <types..., type> ();
    }

    template <typename dialer, uint8_t channels> template <typename type, typename... types> constexpr bool multiplexer <dialer, channels> :: traits :: is_forward_constructible()
    {
        return (std :: is_same <type, pool &> :: value) && (std :: is_constructible <dialer, types...> :: value);
    }

    template <typename dialer, uint8_t channels> template <typename type, typename... types> pool & multiplexer <dialer, channels> :: traits :: leech(type && arg, types && ... args)
    {
        if constexpr (std :: is_same <type, pool &> :: value)
            return arg;
        else
            return leech(args...);
    }

    // Constraints

    template <typename dialer, uint8_t channels> template <typename... types> constexpr bool multiplexer <dialer, channels> :: constraints :: leech()
    {
        if constexpr (sizeof...(types) == 0)
            return false;
        else
            return traits :: template is_leech_constructible <types...> ();
    }

    template <typename dialer, uint8_t channels> template <typename... types> constexpr bool multiplexer <dialer, channels> :: constraints :: rotate()
    {
        if constexpr (sizeof...(types) == 0)
            return false;
        else
            return traits :: template is_rotate_constructible <types...> ();
    }

    template <typename dialer, uint8_t channels> template <typename... types> constexpr bool multiplexer <dialer, channels> :: constraints :: forward()
    {
        if constexpr (sizeof...(types) == 0)
            return false;
        else
            return traits :: template is_forward_constructible <types...> ();
    }

    // emitter

    // Methods

    template <typename dialer, uint8_t channels> template <typename channel> void multiplexer <dialer, channels> :: emitter :: emit(const dial & dial)
    {
        this->eventemitter <channel, class dial> :: template emit <channel> (dial);
    }

    template <typename dialer, uint8_t channels> template <typename channel, typename lambda> void multiplexer <dialer, channels> :: emitter :: on(const lambda & handler)
    {
        this->eventemitter <channel, class dial> :: template on <channel> (handler);
    }

    // multiplexer

    // Constructors

    template <typename dialer, uint8_t channels> template <typename... types, std :: enable_if_t <multiplexer <dialer, channels> :: constraints :: template leech <types...> ()> *> multiplexer <dialer, channels> :: multiplexer(types && ... args) : _dialer(args...), _pool(traits :: leech(args...))
    {
        this->init();
    }

    template <typename dialer, uint8_t channels> template <typename type, typename... types, std :: enable_if_t <multiplexer <dialer, channels> :: constraints :: template rotate <type, types...> () && !(multiplexer <dialer, channels> :: constraints :: template forward <type, types...> ())> *> multiplexer <dialer, channels> :: multiplexer(type && arg, types && ... args) : multiplexer(args..., arg)
    {
    }

    template <typename dialer, uint8_t channels> template <typename type, typename... types, std :: enable_if_t <multiplexer <dialer, channels> :: constraints :: template forward <type, types...> ()> *> multiplexer <dialer, channels> :: multiplexer(type && arg, types && ... args) : _dialer(args...), _pool(arg)
    {
        this->init();
    }

    // Getters

    template <typename dialer, uint8_t channels> const identifier & multiplexer <dialer, channels> :: identifier() const
    {
        return this->_dialer.identifier();
    }

    template <typename dialer, uint8_t channels> signer & multiplexer <dialer, channels> :: signer()
    {
        return this->_dialer.signer();
    }

    template <typename dialer, uint8_t channels> keyexchanger & multiplexer <dialer, channels> :: keyexchanger()
    {
        return this->_dialer.keyexchanger();
    }

    // Methods

    template <typename dialer, uint8_t channels> template <uint8_t idx, std :: enable_if_t <(idx < channels)> *> promise <dial> multiplexer <dialer, channels> :: connect(const vine :: identifier & identifier)
    {
        return this->_dialer.connect(identifier).then([=](const dial & dial) -> promise <class dial>
        {
            co_await this->_pool.bind(dial).send(idx);
            co_return dial;
        });
    }

    template <typename dialer, uint8_t channels> template <uint8_t idx, typename lambda, std :: enable_if_t <eventemitter <channel <idx>, dial> :: constraints :: template callback <lambda> ()> *> void multiplexer <dialer, channels> :: on(const lambda & handler)
    {
        this->_emitter.template on <channel <idx>> (handler);
    }

    // Private methods

    template <typename dialer, uint8_t channels> void multiplexer <dialer, channels> :: init()
    {
        this->_dialer.template on <dial> ([=](dial dial) -> promise <void>
        {
            uint8_t channel = co_await this->_pool.bind(dial).template receive <uint8_t> ();

            if(channel < channels)
                this->dispatch <channels - 1> (channel, dial);
        });
    }

    template <typename dialer, uint8_t channels> template <uint8_t haystack> void multiplexer <dialer, channels> :: dispatch(const uint8_t & needle, const dial & dial)
    {
        if(needle != haystack)
        {
            if constexpr ((haystack > 0))
                this->dispatch <haystack - 1> (needle, dial);
        }
        else
            this->_emitter.template emit <channel <haystack>> (dial);
    }
};

#endif
