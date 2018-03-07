#ifndef __drop__data__heap__hpp
#define __drop__data__heap__hpp

// Includes

#include "heap.h"

namespace drop
{
    // Settings

    template <typename type> constexpr size_t heap <type> :: settings :: base_alloc;

    // Constructors

    template <typename type> heap <type> :: heap() : _items(new optional <type> [settings :: base_alloc + 1]), _size(0), _alloc(settings :: base_alloc)
    {
    }

    // Destructor

    template <typename type> heap <type> :: ~heap()
    {
        delete [] this->_items;
    }

    // Getters

    template <typename type> const size_t & heap <type> :: size() const
    {
        return this->_size;
    }

    // Methods

    template <typename type> void heap <type> :: push(const type & item)
    {
        if(this->_size == this->_alloc)
        {
            optional <type> * old = this->_items;
            this->_alloc *= 2;

            this->_items = new optional <type> [this->_alloc + 1];

            for(size_t i = 1; i <= this->_size; i++)
                this->_items[i] = old[i];

            delete [] old;
        }

        this->_items[++(this->_size)] = item;
        this->bubble_up(this->_size);
    }

    template <typename type> const type & heap <type> :: peek() const
    {
        assert(this->_size);
        return *(this->_items[1]);
    }

    template <typename type> type heap <type> :: pop()
    {
        type item = *(this->_items[1]);

        this->_items[1] = this->_items[this->_size];
        this->_items[(this->_size)--] = null;

        this->bubble_down(1);

        return item;
    }

    // Private methods

    template <typename type> void heap <type> :: swap(const size_t & i, const size_t & j)
    {
        optional <type> swap = std :: move(this->_items[i]);
        this->_items[i] = std :: move(this->_items[j]);
        this->_items[j] = std :: move(swap);
    }

    template <typename type> void heap <type> :: bubble_up(const size_t & i)
    {
        if(i > 1 && *(this->_items[i]) < *(this->_items[i / 2]))
        {
            this->swap(i, i / 2);
            this->bubble_up(i / 2);
        }
    }

    template <typename type> void heap <type> :: bubble_down(const size_t & i)
    {
        if(i * 2 < this->_size)
        {
            if(*(this->_items[i]) > *(this->_items[i * 2]) || *(this->_items[i]) > *(this->_items[i * 2 + 1]))
            {
                size_t target = (*(this->_items[i * 2]) < *(this->_items[i * 2 + 1])) ? (i * 2) : (i * 2 + 1);
                this->swap(i, target);
                this->bubble_down(target);
            }
        }
        else if(i * 2 == this->_size)
        {
            if(*(this->_items[i]) > *(this->_items[i * 2]))
                this->swap(i, i * 2);
        }
    }
};

#endif
