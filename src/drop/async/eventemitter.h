// Forward declarations

namespace drop
{
    template <typename, typename...> class eventemitter;
};

#if !defined(__forward__) && !defined(__drop__async__eventemitter__h)
#define __drop__async__eventemitter__h

// Libraries

#include <vector>
#include <mutex>

// Includes

#include "drop/utils/sfinae.h"

namespace drop
{
    template <typename event, typename... types> class eventemitter
    {
        // Traits

        struct traits
        {
            template <typename lambda> static constexpr bool is_types_callable();
        };

    public:

        // Constraints

        struct constraints
        {
            template <typename lambda> static constexpr bool callback();
        };

    private:

        // Service nested classes

        class callback_interface
        {
        public:

            // Destructor

            virtual ~callback_interface();

            // Operators

            virtual bool operator () (const types & ...) = 0;
        };

        template <typename lambda> class callback : public callback_interface
        {
            // Members

            lambda _callback;

        public:

            // Constructors

            callback(const lambda &);

            // Operators

            bool operator () (const types & ...);
        };

        // Members

        std :: vector <callback_interface *> _callbacks;
        std :: mutex _mutex;

    public:

        // Destructor

        ~eventemitter();

        // Methods

        template <typename etype, typename lambda, std :: enable_if_t <(std :: is_same <etype, event> :: value) && (constraints :: template callback <lambda> ())> * = nullptr> void on(const lambda &);

        void clear();

    protected:

        // Protected methods

        template <typename etype, std :: enable_if_t <std :: is_same <etype, event> :: value> * = nullptr> bool emit(const types & ...);
    };
};

#endif
