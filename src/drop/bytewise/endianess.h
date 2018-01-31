// Forward declarations

namespace drop
{
    class endianess;
};

#if !defined(__forward__) && !defined(__drop__bytewise__endianess__h)
#define __drop__bytewise__endianess__h

// Libraries

#include <stdint.h>
#include <type_traits>

namespace drop
{
    class endianess
    {
    public:

        // Nested enums

        enum type {little, big};

        // Macros

        #ifdef __BYTE_ORDER
            #if __BYTE_ORDER == __BIG_ENDIAN
                #define __drop__bytewise__endianess__endianess big
            #endif

            #if __BYTE_ORDER == __LITTLE_ENDIAN
                #define __drop__bytewise__endianess__endianess little
            #endif
        #endif

        #ifdef __BIG_ENDIAN__
            #define __drop__bytewise__endianess__endianess big
        #endif

        #ifdef __LITTLE_ENDIAN__
            #define __drop__bytewise__endianess__endianess little
        #endif

        #ifdef __ARMEB__
            #define __drop__bytewise__endianess__endianess big
        #endif

        #ifdef __ARMEL__
            #define __drop__bytewise__endianess__endianess little
        #endif

        #ifdef __THUMBEB__
            #define __drop__bytewise__endianess__endianess big
        #endif

        #ifdef __THUMBEL__
            #define __drop__bytewise__endianess__endianess little
        #endif

        #ifdef __AARCH64EB__
            #define __drop__bytewise__endianess__endianess big
        #endif

        #ifdef __AARCH64EL__
            #define __drop__bytewise__endianess__endianess little
        #endif

        #ifdef _MIPSEB
            #define __drop__bytewise__endianess__endianess big
        #endif

        #ifdef _MIPSEL
            #define __drop__bytewise__endianess__endianess little
        #endif

        #ifdef __MIPSEB
            #define __drop__bytewise__endianess__endianess big
        #endif

        #ifdef __MIPSEL
            #define __drop__bytewise__endianess__endianess little
        #endif

        #ifdef __MIPSEB__
            #define __drop__bytewise__endianess__endianess big
        #endif

        #ifdef __MIPSEL__
            #define __drop__bytewise__endianess__endianess little
        #endif

        #ifdef __BYTE_ORDER__
            #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
                #define __drop__bytewise__endianess__endianess big
            #endif

            #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
                #define __drop__bytewise__endianess__endianess little
            #endif
        #endif

        #ifdef __FLOAT_WORD_ORDER__
            #if __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__
                #define __drop__bytewise__endianess__endianess big
            #endif

            #if __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__
                #define __drop__bytewise__endianess__endianess little
            #endif
        #endif

        // Static members

        static constexpr type local = __drop__bytewise__endianess__endianess;
        static constexpr type network = little;

        // Static methods

        static inline int8_t swap(const int8_t &);
        static inline uint8_t swap(const uint8_t &);
        static inline int16_t swap(const int16_t &);
        static inline uint16_t swap(const uint16_t &);
        static inline int32_t swap(const int32_t &);
        static inline uint32_t swap(const uint32_t &);
        static inline int64_t swap(const int64_t &);
        static inline uint64_t swap(const uint64_t &);

        template <type target, typename vtype, std :: enable_if_t <std :: is_integral <vtype> :: value> * = nullptr> static inline vtype to(const vtype &);
        template <typename vtype, std :: enable_if_t <std :: is_integral <vtype> :: value> * = nullptr> static inline vtype translate(const vtype &);
    };
};

#endif
