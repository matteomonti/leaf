// Forward declarations

namespace drop
{
    template <typename> class heap;
};

#if !defined(__forward__) && !(__drop__data__heap__h)
#define __drop__data__heap__h

// Libraries

#include <stddef.h>

// Includes

#include "optional.hpp"

namespace drop
{
    template <typename type> class heap
    {
        // Settings

        struct settings
        {
            static constexpr size_t base_alloc = 32;
        };

        // Members

        optional <type> * _items;
        size_t _size;
        size_t _alloc;

    public:

        // Constructors

        heap();

        // Destructor

        ~heap();

        // Getters

        const size_t & size() const;

        // Methods

        void push(const type &);

        const type & peek() const;
        type pop();

    private:

        // Private methods

        void swap(const size_t &, const size_t &);
        void bubble_up(const size_t &);
        void bubble_down(const size_t &);
    };
};

#endif
