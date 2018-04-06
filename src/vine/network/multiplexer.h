// Forward declarations

namespace vine
{
    template <uint8_t> class channel;
    template <typename, uint8_t> class multiplexer;
};

#if !defined(__forward__) && !defined(__vine__network__multiplexer__h)
#define __vine__network__multiplexer__h

// Libraries

#include <type_traits>

// Includes

#include "dial.h"
#include "drop/async/eventemitter.hpp"
#include "drop/async/promise.hpp"
#include "drop/network/pool.hpp"

namespace vine
{
    using namespace drop;

    template <uint8_t> class channel
    {
    };

    template <typename dialer, uint8_t channels> class multiplexer
    {
        // Traits

        struct traits
        {
            template <typename... types> static constexpr bool is_leech_constructible();
            template <typename type, typename... types> static constexpr bool is_rotate_constructible();
            template <typename type, typename... types> static constexpr bool is_forward_constructible();

            template <typename type, typename... types> static pool & leech(type &&, types && ...);
        };

    public:

        // Constraints

        struct constraints
        {
            template <typename... types> static constexpr bool leech();
            template <typename... types> static constexpr bool rotate();
            template <typename... types> static constexpr bool forward();
        };

    private:

        // Service nested classes

        template <uint8_t, bool> class emitter_base;

        template <bool _> class emitter_base <0, _> : public eventemitter <channel <0>, dial>
        {
        };

        template <uint8_t index, bool _> class emitter_base : public emitter_base <index - 1, _>, public eventemitter <channel <index>, dial>
        {
        };

        class emitter : public emitter_base <channels - 1, false>
        {
        public:

            // Methods

            template <typename channel> void emit(const dial &);
            template <typename channel, typename lambda> void on(const lambda &);
        };

        // Members

        dialer _dialer;
        emitter _emitter;

        pool & _pool;

    public:

        // Constructors

        template <typename... types, std :: enable_if_t <constraints :: template leech <types...> ()> * = nullptr> multiplexer(types && ...);
        template <typename type, typename... types, std :: enable_if_t <constraints :: template rotate <type, types...> () && !(constraints :: template forward <type, types...> ())> * = nullptr> multiplexer(type &&, types && ...);
        template <typename type, typename... types, std :: enable_if_t <constraints :: template forward <type, types...> ()> * = nullptr> multiplexer(type &&, types && ...);

        // Getters

        const identifier & identifier() const;
        signer & signer();
        keyexchanger & keyexchanger();

        // Methods

        template <uint8_t idx, std :: enable_if_t <(idx < channels)> * = nullptr> promise <dial> connect(const vine :: identifier &);
        template <uint8_t idx, typename lambda, std :: enable_if_t <eventemitter <channel <idx>, dial> :: constraints :: template callback <lambda> ()> * = nullptr> void on(const lambda &);

    private:

        // Private methods

        void init();
        template <uint8_t haystack> void dispatch(const uint8_t &, const dial &);
    };
};

#endif
