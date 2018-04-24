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

        // Service nested structs forward declarations

        struct command;
        struct publication;
        struct subscription;

        // Nested classes

        class archive
        {
            // Members

            const ttype * _topic;
            const messenger <command, publication> * _messenger;
            optional <bool *> _sent;

            // Constructors

            archive(const ttype &, const messenger <command, publication &>);
            archive(const ttype &, const messenger <command, publication> &, bool &);

            // Operators

            const archive & operator << (const ptype &) const;
        };

        class emitter : public eventemitter <archive, archive>
        {
            template <typename, typename> friend class gossiper;
        };

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

            ttype topic;
            id id;
            bool once;

            // Constructors

            subscription(const ttype &, const publisher :: id &, const bool &);

            // Methods

            template <typename vtype> void accept(bytewise :: reader <vtype> &) const;

            // Operators

            bool operator == (const subscription &) const;
        };

        // Members

        std :: unordered_map <id, messenger <command, publication>> _messengers;

        std :: unordered_map <ttype, std :: shared_ptr <std :: unordered_set <subscription, shorthash>>, shorthash> _topics;
        std :: unordered_map <id, std :: shared_ptr <std :: unordered_set <subscription, shorthash>>, shorthash> _sessions;

        id _nonce;

        std :: mutex _mutex;

        emitter _emitter;
        crontab & _crontab;

    public:

        // Constructors

        publisher(crontab &);

        // Methods

        void serve(const pool :: connection &);

        template <typename type, typename lambda, std :: enable_if_t <(std :: is_same <type, archive> :: value) && (eventemitter <archive, archive> :: constraints :: template callback <lambda> ())> * = nullptr> void on(const lambda &);

    private:

        // Private methods

        void command(const id &, const command &);

        void add(const subscription &);
        void remove(const subscription &);

        void clear(const id &);
        void drop(const id &);

        void remove_from_topic(const subscription &);
        void remove_from_session(const subscription &);
    };
};

#endif
