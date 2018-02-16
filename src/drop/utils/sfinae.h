// Forward declarations

namespace drop
{
    class sfinae;
};

#if !defined(__forward__) && !defined(__drop__utils__sfinae__h)
#define __drop__utils__sfinae__h

// Libraries

#include <type_traits>

namespace drop
{
    class sfinae
    {
        // Service nested classes

        struct any
        {
        };

        template <typename expression, typename rettype> class evaluator
        {
        public:

            // Methods

            template <typename type> constexpr bool satisfied() const;

        private:

            // Private methods

            template <typename type> constexpr auto test(int) const -> decltype(std :: declval <expression> ()(std :: declval <type> ()), std :: true_type());
            template <typename type> constexpr std :: false_type test(...) const;
        };

    public:

        // Static methods

        template <typename expression> static constexpr auto is_valid(const expression &);
        template <typename rettype, typename expression> static constexpr auto returns(const expression &);
    };
};

#endif
