#ifndef __poseidon__poseidon__checkpool__hpp
#define __poseidon__poseidon__checkpool__hpp

// Includes

#include "checkpool.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Methods

    template <size_t size> size_t checkpool <size> :: init(const std :: unordered_set <index, shorthash> & indexes)
    {
        this->_version++;

        this->_indexes.clear();
        this->_indexes.reserve(indexes.size());

        for(const index & index : indexes)
            this->_indexes.push_back(index);

        return this->_version;
    }

    template <size_t size> void checkpool <size> :: set(const size_t & version, const size_t & slot, const std :: vector <optional <value>> & responses)
    {
        if(version != this->_version)
            return;

        this->_slots[slot] = responses;
    }

    template <size_t size> template <size_t threshold, typename alambda, typename rlambda> void checkpool <size> :: evaluate(const alambda & accept, const rlambda & reject)
    {
        for(size_t index = 0; index < this->_indexes.size(); index++)
        {
            std :: unordered_map <value, size_t, shorthash> scores;

            for(size_t slot = 0; slot < size; slot++)
            {
                if(this->_slots[slot].size() > 0 && this->_slots[slot][index])
                {
                    try
                    {
                        size_t score = scores.at(*(this->_slots[slot][index]));
                        scores[*(this->_slots[slot][index])] = score + 1;
                    }
                    catch(...)
                    {
                        scores[*(this->_slots[slot][index])] = 1;
                    }
                }
            }

            struct
            {
                value value;
                size_t score = 0;
            } best;

            size_t total = 0;

            for(const auto & score : scores)
            {
                if(score.second > best.score)
                {
                    best.value = score.first;
                    best.score = score.second;
                }

                total += score.second;
            }

            if(best.score > threshold)
                accept(statement(this->_indexes[index], best.value.value, best.value.signature)); // TODO: Use improved constructor.
            else if((total - best.score) > (size - threshold))
                reject(this->_indexes[index]);
        }
    }
};

#endif
