// Forward declarations

namespace drop
{
    template <typename, typename> class publisher;
    template <typename, typename> class subscriber;
};

#if !defined(__forward__) && !defined(__drop__distributed__publisher__h)
#define __drop__distributed__publisher__h

// Libraries

#include <unordered_map>
#include <unordered_set>

// Includes

#include "messenger.hpp"
#include "drop/crypto/shorthash.hpp"
#include "drop/bytewise/bytewise.hpp"

namespace drop
{
    template <typename ttype, typename ptype> class publisher
    {
    public:

        // Constraints

        struct constraints
        {
            static constexpr bool valid();
        };

    private:

        // Friends

        template <typename, typename> class subscriber;

        // Service typedefs

        typedef uint64_t id;

        // Service nested enums

        struct commands
        {
            enum type {subscribe, unsubscribe, once};
        };

    public: // REMOVE ME

        // Service nested structs

        struct command
        {
            // Public members

            ttype topic;
            uint8_t type;

            // Constructors

            command() = default;
            command(const ttype &, const typename commands :: type &);

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
            template <typename vtype> void accept(bytewise :: writer <vtype> &);
        };

        struct publication
        {
            // Public members

            ttype topic;
            ptype payload;

            // Constructors

            publication() = default;
            publication(const ttype &, const ptype &);

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;
            template <typename vtype> void accept(bytewise :: writer <vtype> &);
        };

        struct subscription
        {
            // Public members

            id id;
            bool once;

            // Constructors

            subscription(const publisher :: id &, const bool &);
        };

        // Members

        std :: unordered_map <id, messenger <command, publication>> _messengers;

        id _nonce;

        std :: mutex _mutex;

        crontab & _crontab;

    public:

        // Constructors

        publisher(crontab &);
    };
};

#endif
