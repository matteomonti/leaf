// Forward declarations

namespace drop
{
    template <typename...> class variant;
};

#if !defined(__forward__) && !defined(__drop__data__variant__h)
#define __drop__data__variant__h

// Includes

#include "drop/bytewise/bytewise.h"
#include "drop/utils/sfinae.h"
#include "drop/utils/static_math.hpp"

namespace drop
{
    template <typename... types> class variant
    {
    public: // REMOVE ME

        // Traits

        struct traits
        {
            struct sfinaes
            {
                template <typename ctype, typename vtype> struct call_operator_accepts_reference
                {
                    template <typename ftype, void (ftype :: *)(vtype &)> struct mhelper {};
                    template <typename ftype, void (ftype :: *)(vtype &) const> struct chelper {};

                    template <typename ftype> static uint8_t sfinae(...);
                    template <typename ftype> static uint32_t sfinae(mhelper <ftype, &ftype :: operator ()> *);
                    template <typename ftype> static uint32_t sfinae(chelper <ftype, &ftype :: operator ()> *);

                    static constexpr bool value = std :: is_same <decltype(sfinae <ctype> (0)), uint32_t> :: value;
                };
            };

            template <typename ctype, typename vtype> static constexpr bool is_callable();
            template <typename ctype, typename vtype> static constexpr bool is_directly_callable();
        };

    public:

        // Constraints

        struct constraints
        {
            template <typename ctype> static constexpr bool callback();
        };

    private:

        // Members

        uint8_t _variant;
        std :: aligned_storage_t <max({sizeof(types)...}), max({alignof(types)...})> _value;
    };
};

#endif
