// Forward declarations

namespace drop
{
    template <typename> class promise;
};

#if !defined(__forward__) && !defined(__drop__async__promise__h)
#define __drop__async__promise__h

// Libraries

#include <memory>
#include <exception>
#include <mutex>
#include <experimental/coroutine>

// Includes

#include "drop/data/optional.h"
#include "drop/utils/sfinae.h"

namespace drop
{
    template <typename type> class promise
    {
        // Settings

        struct settings
        {
            static constexpr size_t max_resolvers = 16;
        };

        // Friends

        template <typename> friend class promise;

        // Traits

        struct traits
        {
            template <typename> struct is_promise;

            template <typename ttype> struct is_promise <promise <ttype>>
            {
                static constexpr bool value = true;
            };

            template <typename ptype> struct is_promise
            {
                static constexpr bool value = false;
            };

            template <typename lambda> static constexpr bool is_callback();
            template <typename lambda> static constexpr bool is_chainable();

            template <typename lambda> static constexpr bool is_handler();

            template <typename lambda> static auto declthen();
            template <typename lambda> using chain = decltype(declthen <lambda> ());
        };

    public:

        // Coroutine interface

        class promise_type;

        // Constraints

        struct constraints
        {
            template <typename lambda> static constexpr bool callback();
            template <typename lambda> static constexpr bool handler();
            template <typename... vtype> static constexpr bool value();
        };

        // Exceptions

        struct exceptions
        {
            class unforwardable_reject : public std :: exception
            {
                const char * what() const throw();
            };

            class already_resolved : public std :: exception
            {
                const char * what() const throw();
            };

            class already_rejected : public std :: exception
            {
                const char * what() const throw();
            };
        };

    private:

        // Forward declarations

        class arc;

        // Service nested classes

        class resolver_interface
        {
        public:

            // Destructor

            virtual ~resolver_interface();

            // Methods

            virtual void resolve(const arc &) = 0;
            virtual void reject(const std :: exception_ptr &) = 0;
        };

        template <typename lambda, bool = traits :: template is_chainable <lambda> ()> class resolver;

        template <typename lambda> class resolver <lambda, false> : public resolver_interface
        {
            // Members

            lambda _callback;

        public:

            // Constructors

            resolver(const lambda &);

            // Methods

            void resolve(const arc &);
            void reject(const std :: exception_ptr &);
        };

        template <typename lambda> class resolver <lambda, true> : public resolver_interface
        {
            // Members

            lambda _callback;
            typename traits :: template chain <lambda> _chain;

        public:

            // Constructors

            resolver(const lambda &);

            // Getters

            const auto & chain() const;

            // Methods

            void resolve(const arc &);
            void reject(const std :: exception_ptr &);
        };

        class rejector_interface
        {
        public:

            // Destructor

            virtual ~rejector_interface();

            // Methods

            virtual void reject(const std :: exception_ptr &) = 0;
        };

        template <typename lambda> class rejector : public rejector_interface
        {
            // Members

            lambda _handler;

        public:

            // Constructors

            rejector(const lambda &);

            // Methods

            virtual void reject(const std :: exception_ptr &);
        };

        class arc
        {
            // Members

            std :: conditional_t <std :: is_same <type, void> :: value, bool, optional <type>> _value;

            std :: shared_ptr <arc> _alias;

            resolver_interface * _resolvers[settings :: max_resolvers];
            size_t _size;

            std :: exception_ptr _exception;
            rejector_interface * _rejector;

            std :: mutex _mutex;

        public:

            // Constructors

            arc();

            // Destructor

            ~arc() noexcept(false);

            // Getters

            const auto & value() const;
            const std :: exception_ptr & exception() const;

            // Methods

            template <typename lambda> typename traits :: template chain <lambda> then(const lambda &);
            template <typename lambda> void except(const lambda &);

            template <typename vtype = class null> void resolve(const vtype & = null);
            template <typename etype> void reject(const etype &);
            void reject(const std :: exception_ptr &);

            template <typename lambda> typename traits :: template chain <lambda> call(const lambda &) const;
            void alias(const std :: shared_ptr <arc> &);

            void lock();
            void unlock();
        };

        // Members

        std :: shared_ptr <arc> _arc;
        optional <std :: experimental :: coroutine_handle <>> _coroutine;

    public:

        // Constructors

        promise();
        promise(const promise &);
        promise(promise &&);

        // Getters

        bool resolved() const;

        // Awaitable interface

        bool await_ready();
        void await_suspend(std :: experimental :: coroutine_handle <>);
        auto await_resume();

        // Methods

        template <typename lambda, std :: enable_if_t <constraints :: template callback <lambda> ()> * = nullptr> auto then(const lambda &) const;
        template <typename lambda, std :: enable_if_t <constraints :: template handler <lambda> ()> * = nullptr> const promise & except(const lambda &) const;

        template <typename... vtype, std :: enable_if_t <constraints :: template value <vtype...> ()> * = nullptr> void resolve(const vtype & ...) const;
        template <typename etype> void reject(const etype &) const;

    private:

        // Private methods

        void alias(const promise &) const;

    public:

        // Operators

        promise & operator = (const promise &);
        promise & operator = (promise &&);
    };

    // Coroutine interface

    template <> class promise <void> :: promise_type
    {
        // Members

        promise <void> _promise;

    public:

        // Object

        inline auto get_return_object();

        // Suspends

        inline auto initial_suspend();
        inline auto final_suspend();

        // Exceptions

        inline void unhandled_exception();

        // Returns

        inline void return_void();
    };

    template <typename type> class promise <type> :: promise_type
    {
        // Members

        promise <type> _promise;

    public:

        // Object

        inline auto get_return_object();

        // Suspends

        inline auto initial_suspend();
        inline auto final_suspend();

        // Exceptions

        inline void unhandled_exception();

        // Returns

        inline void return_value(const type &);
    };
}

#endif
