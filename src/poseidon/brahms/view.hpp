#ifndef __poseidon__brahms__view__hpp
#define __poseidon__brahms__view__hpp

// Includes

#include "view.h"
#include "pushpool.hpp"
#include "pullpool.hpp"
#include "sample.hpp"

namespace poseidon
{
    // Constructors

    template <size_t size> view <size> :: view(const std :: array <identifier, size> & that)
    {
        ((std :: array <identifier, size> &) (*this)) = that;
        std :: sort(this->begin(), this->end());
    }

    // Methods

    template <size_t size> template <typename lambda> void view <size> :: distinct(const lambda & visitor) const
    {
        for(size_t i = 0; i < size; this->increment(i))
            visitor((*this)[i]);
    }

    template <size_t size> template <typename alambda, typename rlambda> void view <size> :: diff(const view & that, const alambda & add, const rlambda & remove) const
    {
        size_t i = 0;
        size_t j = 0;

        while((i < size) && (j < size))
        {
            if((*this)[i] < that[j])
            {
                remove((*this)[i]);
                this->increment(i);
            }
            else if((*this)[i] > that[j])
            {
                add(that[j]);
                that.increment(j);
            }
            else
            {
                this->increment(i);
                that.increment(j);
            }
        }

        while(i < size)
        {
            remove((*this)[i]);
            this->increment(i);
        }

        while(j < size)
        {
            add(that[j]);
            that.increment(j);
        }
    }

    template <size_t size> const identifier & view <size> :: random() const
    {
        return (*this)[randombytes_uniform(size)];
    }

    // Private methods

    template <size_t size> void view <size> :: increment(size_t & index) const
    {
        do index++;
        while((index < size) && ((*this)[index] == (*this)[index - 1]));
    }

    // Static methods

    template <size_t size> template <size_t alpha, size_t beta, size_t gamma, size_t pushsize, size_t pullpsize, size_t pullvsize, size_t samplesize> view <size> view <size> :: from(pushpool <pushsize> & pushpool, pullpool <pullpsize, pullvsize> & pullpool, sample <samplesize> & sample)
    {
        std :: array <identifier, size> view;

        for(size_t i = 0; i < alpha; i++)
            view[i] = pushpool.random();

        for(size_t i = alpha; i < alpha + beta; i++)
            view[i] = pullpool.random();

        for(size_t i = alpha + beta; i < alpha + beta + gamma; i++)
            view[i] = sample.random();

        return (class view){view};
    }
};

#endif
