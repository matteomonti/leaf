#ifndef __drop__data__syncset__hpp
#define __drop__data__syncset__hpp

// Includes

#include "syncset.h"
#include "drop/bytewise/bytewise.hpp"
#include "drop/crypto/hash.hpp"
#include "variant.hpp"

namespace drop
{
    // prefix

    // Constructors

    template <typename type> syncset <type> :: prefix :: prefix(const hash & hash, const size_t & bits) : _bits(bits)
    {
        new (this->_value) class hash(hash);
    }

    template <typename type> syncset <type> :: prefix :: prefix(const type & element, const size_t & bits) : _bits(bits)
    {
        new (this->_value) hash(element);
    }

    // Getters

    template <typename type> const size_t & syncset <type> :: prefix :: bits() const
    {
        return this->_bits;
    }

    // Setters

    template <typename type> void syncset <type> :: prefix :: bits(const size_t & bits)
    {
        this->_bits = bits;
    }

    // Methods

    template <typename type> template <typename vtype> void syncset <type> :: prefix :: visit(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_bits);
        reader.update(this->_value, (this->_bits + 7) / 8);
    }

    template <typename type> template <typename vtype> void syncset <type> :: prefix :: visit(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_bits);
        size_t bytes = (this->_bits + 7) / 8;
        memcpy(this->_value, writer.pop(bytes), bytes);
    }

    // Operators

    template <typename type> bool syncset <type> :: prefix :: operator [] (const size_t & index) const
    {
        size_t byte = index / 8;
        size_t bit = index % 8;

        return (this->_value[byte]) & (1 << (7 - bit));
    }

    // navigator

    // Constructors

    template <typename type> syncset <type> :: navigator :: navigator(const prefix & prefix, node & root) : _prefix(prefix), _depth(0)
    {
        this->_trace[this->_depth] = &root;
    }

    // Getters

    template <typename type> const size_t & syncset <type> :: navigator :: depth() const
    {
        return this->_depth;
    }

    template <typename type> typename syncset <type> :: navigation syncset <type> :: navigator :: next() const
    {
        return this->_prefix[this->_depth] ? left : right;
    }

    // Operators

    template <typename type> typename syncset <type> :: node & syncset <type> :: navigator :: operator * ()
    {
        return *(this->_trace[this->_depth]);
    }

    template <typename type> typename syncset <type> :: node * syncset <type> :: navigator :: operator -> ()
    {
        return this->_trace[this->_depth];
    }

    template <typename type> typename syncset <type> :: navigator & syncset <type> :: navigator :: operator ++ ()
    {
        this->_trace[this->_depth]->match([&](const multiple & multiple)
        {
            this->_trace[this->_depth + 1] = (this->next() == left) ? (multiple.left()) : (multiple.right());
        }, [&](const auto &)
        {
            this->_trace[this->_depth + 1] = nullptr;
        });

        this->_depth++;
        return *this;
    }

    template <typename type> typename syncset <type> :: navigator & syncset <type> :: navigator :: operator ++ (int)
    {
        return ++(*this);
    }

    template <typename type> typename syncset <type> :: navigator & syncset <type> :: navigator :: operator -- ()
    {
        this->_depth--;
        return *this;
    }

    template <typename type> typename syncset <type> :: navigator & syncset <type> :: navigator :: operator -- (int)
    {
        return --(*this);
    }

    // Casting

    template <typename type> syncset <type> :: navigator :: operator bool () const
    {
        return (this->_trace[this->_depth] != nullptr);
    }

    // multiple

    // Constructors

    template <typename type> syncset <type> :: multiple :: multiple(const node & left, const node & right) : _left(new node(left)), _right(new node(right))
    {
        this->relabel();
    }

    template <typename type> syncset <type> :: multiple :: multiple(const multiple & that) : _label(that._label), _left(new node(*(that._left))), _right(new node(*(that._right)))
    {
    }

    template <typename type> syncset <type> :: multiple :: multiple(multiple && that) : _label(that._label), _left(that._left), _right(that._right)
    {
        that._left = nullptr;
        that._right = nullptr;
    }

    // Destructor

    template <typename type> syncset <type> :: multiple :: ~multiple()
    {
        if(this->_left && this->_right)
        {
            delete this->_left;
            delete this->_right;
        }
    }

    // Getters

    template <typename type> const hash & syncset <type> :: multiple :: label() const
    {
        return this->_label;
    }

    template <typename type> typename syncset <type> :: node * syncset <type> :: multiple :: left() const
    {
        return this->_left;
    }

    template <typename type> typename syncset <type> :: node * syncset <type> :: multiple :: right() const
    {
        return this->_right;
    }

    // Methods

    template <typename type> void syncset <type> :: multiple :: relabel()
    {
        hasher hasher;

        this->_left->visit([&](auto & left)
        {
            hasher.update(left.label());
        });

        this->_right->visit([&](auto & right)
        {
            hasher.update(right.label());
        });

        this->_label = hasher.finalize();
    }

    template <typename type> bool syncset <type> :: multiple :: pullable()
    {
        return (this->_left->template is <empty> () && this->_right->template is <single> ()) || (this->_left->template is <single> () && this->_right->template is <empty> ());
    }

    template <typename type> typename syncset <type> :: single syncset <type> :: multiple :: pull()
    {
        if(this->_left->template is <single> ())
            return this->_left->template reinterpret <single> ();
        else
            return this->_right->template reinterpret <single> ();
    }

    // Operators

    template <typename type> typename syncset <type> :: multiple & syncset <type> :: multiple :: operator = (const multiple & rho)
    {
        this->~multiple();

        this->_label = rho._label;
        this->_left = new node(*(rho._left));
        this->_right = new node(*(rho._right));

        return (*this);
    }

    template <typename type> typename syncset <type> :: multiple & syncset <type> :: multiple :: operator = (multiple && rho)
    {
        this->_label = rho._label;
        this->_left = rho._left;
        this->_right = rho._right;

        rho._left = nullptr;
        rho._right = nullptr;

        return (*this);
    }

    // single

    // Constructors

    template <typename type> syncset <type> :: single :: single(const type & element) : _label(element), _element(element)
    {
    }

    // Getters

    template <typename type> const hash & syncset <type> :: single :: label() const
    {
        return this->_label;
    }

    template <typename type> const type & syncset <type> :: single :: element() const
    {
        return this->_element;
    }

    // Methods

    template <typename type> typename syncset <type> :: empty syncset <type> :: single :: empty()
    {
        return empty();
    }

    template <typename type> typename syncset <type> :: multiple syncset <type> :: single :: push(const navigation & child)
    {
        return (child == left) ? multiple(*this, (class empty){}) : multiple((class empty){}, *this);
    }

    // empty

    // Getters

    template <typename type> const hash & syncset <type> :: empty :: label() const
    {
        return this->_label;
    }

    // Methods

    template <typename type> typename syncset <type> :: single syncset <type> :: empty :: fill(const type & element)
    {
        return single(element);
    }

    // node

    // Constructors

    template <typename type> syncset <type> :: node :: node(const multiple & node) : variant <multiple, single, empty> (node)
    {
    }

    template <typename type> syncset <type> :: node :: node(const single & node) : variant <multiple, single, empty> (node)
    {
    }

    template <typename type> syncset <type> :: node :: node(const empty & node) : variant <multiple, single, empty> (node)
    {
    }

    // Methods

    template <typename type> void syncset <type> :: node :: relabel()
    {
        this->match([](multiple & multiple)
        {
            multiple.relabel();
        });
    }

    // syncset

    // Constructors

    template <typename type> syncset <type> :: syncset() : _root(empty())
    {
    }

    // Methods

    template <typename type> void syncset <type> :: add(const type & element)
    {
        prefix prefix(element);
        navigator navigator(prefix, this->_root);

        for(; navigator; navigator++)
        {
            navigator->match([&](empty & empty)
            {
                (*navigator) = empty.fill(element);
            }, [&](single & single)
            {
                class prefix singleprefix(single.label());
                (*navigator) = single.push(singleprefix[navigator.depth()] ? left : right);
            });
        }

        navigator--;

        while(true)
        {
            if(navigator.depth() != 0)
                navigator--;
            else
                return;

            navigator->relabel();
        }
    }

    // Static declarations

    template <typename type> hash syncset <type> :: empty :: _label = hasher().finalize();
};

#endif
