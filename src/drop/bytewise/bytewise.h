// Forward declarations

namespace drop
{
    class bytewise;
};

#if !defined(__forward__) && !defined(__drop__bytewise__bytewise__h)
#define __drop__bytewise__bytewise__h

// Libraries

#include <type_traits>
#include <vector>

// Forward includes

#define __forward__
#include "drop/data/buffer.h"
#undef __forward__

// Includes

#include "endianess.h"

namespace drop
{
    class bytewise
    {
    public:

        // Forward declarations

        template <typename vtype> class reader;
        template <typename vtype> class writer;

    private:

        // Traits

        struct traits
        {
            template <typename atype, typename vtype> struct can_accept_reader
            {
            	template <typename stype, void (stype :: *) (reader <vtype> &) const> struct sfinae{};

            	template <typename ttype> static std :: true_type test(sfinae <ttype, &ttype :: accept> *);
            	template <typename ttype> static std :: false_type test(...);

            	static constexpr bool value = std :: is_same <std :: true_type, decltype(test <atype> (nullptr))> :: value;
            };

            template <typename atype, typename vtype> struct can_accept_writer
            {
            	template <typename stype, void (stype :: *) (writer <vtype> &)> struct sfinae{};

            	template <typename ttype> static std :: true_type test(sfinae <ttype, &ttype :: accept> *);
            	template <typename ttype> static std :: false_type test(...);

            	static constexpr bool value = std :: is_same <std :: true_type, decltype(test <atype> (nullptr))> :: value;
            };

            template <typename utype> struct can_update
            {
            	template <typename stype, void (stype :: *) (const uint8_t *, const size_t &)> struct sfinae{};

            	template <typename ttype> static std :: true_type test(sfinae <ttype, &ttype :: update> *);
            	template <typename ttype> static std :: false_type test(...);

            	static constexpr bool value = std :: is_same <std :: true_type, decltype(test <utype> (nullptr))> :: value;
            };

            template <typename utype> struct can_pop
            {
                template <typename stype, const uint8_t * (stype :: *) (const size_t &)> struct sfinae{};

            	template <typename ttype> static std :: true_type test(sfinae <ttype, &ttype :: pop> *);
            	template <typename ttype> static std :: false_type test(...);

            	static constexpr bool value = std :: is_same <std :: true_type, decltype(test <utype> (nullptr))> :: value;
            };

            template <typename vtype> struct is_vector;
            template <typename itype> struct is_vector <std :: vector <itype>>
            {
                static constexpr bool value = true;
                typedef itype type;
            };

            template <typename vtype> struct is_vector
            {
                static constexpr bool value = false;
            };
        };

    public:

        // Constraints

        struct constraints
        {
            template <typename atype, typename vtype> static constexpr bool readable();
            template <typename atype, typename vtype> static constexpr bool writable();

            template <typename vtype> static constexpr bool reader();
            template <typename vtype> static constexpr bool writer();

            template <typename atype> static constexpr bool serializable();
            template <typename atype> static constexpr bool deserializable();
        };

        // Nested classes

        template <typename vtype> class reader
        {
            // Asserts

            static_assert(constraints :: reader <vtype> (), "reader must implement an update(const uint8_t *, const size_t &) interface.");

            // Members

            vtype & _reader;

        public:

            // Constructors

            reader(vtype &);

            // Methods

            inline reader & update(const uint8_t *, const size_t &);

            // Operators

            template <typename atype, std :: enable_if_t <constraints :: readable <atype, vtype> ()> * = nullptr> inline reader & operator << (const atype &);
        };

        template <typename vtype> class writer
        {
            // Asserts

            static_assert(constraints :: writer <vtype> (), "writer must implement a const uint8_t * pop(const size_t &) interface.");

            // Members

            vtype & _writer;

        public:

            // Constructors

            writer(vtype &);

            // Methods

            inline const uint8_t * pop(const size_t &);

            // Operators

            template <typename atype, std :: enable_if_t <constraints :: writable <atype, vtype> ()> * = nullptr> inline writer & operator >> (atype &);
        };

    private:

        // Service nested classes

        struct sizer
        {
            // Members

            size_t size;

            // Constructors

            inline sizer();

            // Methods

            inline void update(const uint8_t *, const size_t &);
        };

        struct rstream
        {
            // Members

            buffer & bytes;
            size_t cursor;

            // Constructors

            inline rstream(buffer &);

            // Methods

            inline void update(const uint8_t *, const size_t &);
        };

        struct wstream
        {
            // Members

            const buffer & bytes;
            size_t cursor;

            // Constructors

            inline wstream(const buffer &);

            // Methods

            inline const uint8_t * pop(const size_t &);
        };

    public:

        // Static methods

        template <typename vtype, typename... atypes, std :: enable_if_t <constraints :: reader <vtype> () && (... && (constraints :: readable <atypes, vtype> ()))> * = nullptr> static inline void read(vtype &, const atypes & ...);

        // TODO: Shift back to using fold expressions as soon as
        // https://bugs.llvm.org/show_bug.cgi?id=32563
        // is fixed.
        template <typename vtype, typename atype, typename... atypes, std :: enable_if_t <constraints :: writer <vtype> () && constraints :: writable <atype, vtype> () && (... && (constraints :: writable <atypes, vtype> ()))> * = nullptr> static inline void write(vtype &, atype &, atypes & ...);

        template <typename... atypes, std :: enable_if_t <(... && (constraints :: serializable <atypes> ()))> * = nullptr> static inline buffer serialize(const atypes & ...);

        template <typename atype, std :: enable_if_t <constraints :: deserializable <atype> ()> * = nullptr> static inline atype deserialize(const buffer &);
        template <typename... atypes, std :: enable_if_t <(sizeof...(atypes) > 1) && (... && (constraints :: deserializable <atypes> ()))> * = nullptr> static inline std :: tuple <atypes...> deserialize(const buffer &);
    };
};

#endif
