// Forward declarations

namespace drop
{
    namespace enablers
    {
        template <bool> class copy_constructible;
        template <bool> class move_constructible;
        template <bool> class copy_assignable;
        template <bool> class move_assignable;
    };
};

#if !defined(__forward__) && !defined(__drop__utils__enablers__h)
#define __drop__utils__enablers__h

namespace drop :: enablers
{
    template <> class copy_constructible <false>
    {
    public:

        // Constructors

        copy_constructible() = default;
        copy_constructible(const copy_constructible &) = delete;
        copy_constructible(copy_constructible &&) = default;

        // Operators

        copy_constructible & operator = (const copy_constructible &) = default;
        copy_constructible & operator = (copy_constructible &&) = default;
    };

    template <> class copy_constructible <true>
    {
    public:

        // Constructors

        copy_constructible() = default;
        copy_constructible(const copy_constructible &) = default;
        copy_constructible(copy_constructible &&) = default;

        // Operators

        copy_constructible & operator = (const copy_constructible &) = default;
        copy_constructible & operator = (copy_constructible &&) = default;
    };

    template <> class move_constructible <false>
    {
    public:

        // Constructors

        move_constructible() = default;
        move_constructible(const move_constructible &) = default;
        move_constructible(move_constructible &&) = delete;

        // Operators

        move_constructible & operator = (const move_constructible &) = default;
        move_constructible & operator = (move_constructible &&) = default;
    };

    template <> class move_constructible <true>
    {
    public:

        // Constructors

        move_constructible() = default;
        move_constructible(const move_constructible &) = default;
        move_constructible(move_constructible &&) = default;

        // Operators

        move_constructible & operator = (const move_constructible &) = default;
        move_constructible & operator = (move_constructible &&) = default;
    };

    template <> class copy_assignable <false>
    {
    public:

        // Constructors

        copy_assignable() = default;
        copy_assignable(const copy_assignable &) = default;
        copy_assignable(copy_assignable &&) = default;

        // Operators

        copy_assignable & operator = (const copy_assignable &) = delete;
        copy_assignable & operator = (copy_assignable &&) = default;
    };

    template <> class copy_assignable <true>
    {
    public:

        // Constructors

        copy_assignable() = default;
        copy_assignable(const copy_assignable &) = default;
        copy_assignable(copy_assignable &&) = default;

        // Operators

        copy_assignable & operator = (const copy_assignable &) = default;
        copy_assignable & operator = (copy_assignable &&) = default;
    };

    template <> class move_assignable <false>
    {
    public:

        // Constructors

        move_assignable() = default;
        move_assignable(const move_assignable &) = default;
        move_assignable(move_assignable &&) = default;

        // Operators

        move_assignable & operator = (const move_assignable &) = default;
        move_assignable & operator = (move_assignable &&) = delete;
    };

    template <> class move_assignable <true>
    {
    public:

        // Constructors

        move_assignable() = default;
        move_assignable(const move_assignable &) = default;
        move_assignable(move_assignable &&) = default;

        // Operators

        move_assignable & operator = (const move_assignable &) = default;
        move_assignable & operator = (move_assignable &&) = default;
    };
};

#endif
