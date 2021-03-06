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

    template <typename type> syncset <type> :: prefix :: prefix() : _value{}, _bits(0)
    {
    }

    template <typename type> syncset <type> :: prefix :: prefix(const hash & hash, const size_t & bits) : _bits(bits)
    {
        new (this->_value) class hash(hash);
    }

    template <typename type> syncset <type> :: prefix :: prefix(const type & element, const size_t & bits) : _bits(bits)
    {
        new (this->_value) hash(element);
    }

    // Getters

    template <typename type> size_t syncset <type> :: prefix :: bits() const
    {
        return this->_bits;
    }

    // Setters

    template <typename type> void syncset <type> :: prefix :: bits(const size_t & bits)
    {
        this->_bits = bits;
    }

    // Methods

    template <typename type> typename syncset <type> :: prefix syncset <type> :: prefix :: left() const
    {
        prefix left = (*this);

        size_t byte = this->_bits / 8;
        size_t bit = this->_bits % 8;

        left._value[byte] &= (~(1 << (7 - bit)));
        left._bits++;

        return left;
    }

    template <typename type> typename syncset <type> :: prefix syncset <type> :: prefix :: right() const
    {
        prefix right = (*this);

        size_t byte = this->_bits / 8;
        size_t bit = this->_bits % 8;

        right._value[byte] |= (1 << (7 - bit));
        right._bits++;

        return right;
    }

    template <typename type> template <typename vtype> void syncset <type> :: prefix :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_bits);
        reader.update(this->_value, (this->_bits + 7) / 8);
    }

    template <typename type> template <typename vtype> void syncset <type> :: prefix :: accept(bytewise :: writer <vtype> & writer)
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
        return this->_prefix[this->_depth] ? right : left;
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
        this->refresh();
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

    template <typename type> const size_t & syncset <type> :: multiple :: size() const
    {
        return this->_size;
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

    template <typename type> void syncset <type> :: multiple :: refresh()
    {
        hasher hasher;
        this->_size = 0;

        this->_left->match([&](multiple & left)
        {
            this->_size += left._size;
            hasher.update(left.label());
        }, [&](single & left)
        {
            this->_size++;
            hasher.update(left.label());
        }, [&](empty & left)
        {
            hasher.update(left.label());
        });

        this->_right->match([&](multiple & right)
        {
            this->_size += right._size;
            hasher.update(right.label());
        }, [&](single & right)
        {
            this->_size++;
            hasher.update(right.label());
        }, [&](empty & right)
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

    template <typename type> template <typename vtype> void syncset <type> :: multiple :: traverse(vtype && visitor)
    {
        this->_left.visit([&](auto & node)
        {
            node.traverse(visitor);
        });

        this->_right.visit([&](auto & node)
        {
            node.traverse(visitor);
        });
    }

    template <typename type> template <typename vtype> void syncset <type> :: multiple :: traverse(vtype && visitor) const
    {
        this->_left->visit([&](const auto & node)
        {
            node.traverse(visitor);
        });

        this->_right->visit([&](const auto & node)
        {
            node.traverse(visitor);
        });
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
        return (class empty){};
    }

    template <typename type> typename syncset <type> :: multiple syncset <type> :: single :: push(const navigation & child)
    {
        return (child == left) ? multiple(*this, (class empty){}) : multiple((class empty){}, *this);
    }

    template <typename type> template <typename vtype> void syncset <type> :: single :: traverse(vtype && visitor)
    {
        visitor(this->_element);
    }

    template <typename type> template <typename vtype> void syncset <type> :: single :: traverse(vtype && visitor) const
    {
        visitor(this->_element);
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

    template <typename type> template <typename vtype> void syncset <type> :: empty :: traverse(vtype &&)
    {
    }

    template <typename type> template <typename vtype> void syncset <type> :: empty :: traverse(vtype &&) const
    {
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

    // labelset

    // Constructors

    template <typename type> syncset <type> :: labelset :: labelset(const class prefix & prefix, const multiple & node) : _prefix(prefix), _label(node.label())
    {
    }

    // Getters

    template <typename type> const typename syncset <type> :: prefix & syncset <type> :: labelset :: prefix() const
    {
        return this->_prefix;
    }

    template <typename type> const hash & syncset <type> :: labelset :: label() const
    {
        return this->_label;
    }

    // Methods

    template <typename type> template <typename vtype> void syncset <type> :: labelset :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_prefix);
        reader << (this->_label);
    }

    template <typename type> template <typename vtype> void syncset <type> :: labelset :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_prefix);
        writer >> (this->_label);
    }

    // Operators

    template <typename type> bool syncset <type> :: labelset :: operator == (const labelset & rho) const
    {
        return this->_label == rho._label;
    }

    template <typename type> bool syncset <type> :: labelset :: operator != (const labelset & rho) const
    {
        return !((*this) == rho);
    }

    // listset

    // Constructors

    template <typename type> syncset <type> :: listset :: listset() : _size(0)
    {
    }

    template <typename type> syncset <type> :: listset :: listset(const class prefix & prefix, const multiple & subroot, const bool & dump) : _prefix(prefix), _size(subroot.size()), _elements(new type[subroot.size()]), _dump(dump)
    {
        size_t cursor = 0;
        subroot.traverse([&](const type & element)
        {
            this->_elements[cursor++] = element;
        });
    }

    template <typename type> syncset <type> :: listset :: listset(const class prefix & prefix, const type & element, const bool & dump) : _prefix(prefix), _elements(new type[1]{element}), _size(1), _dump(dump)
    {
    }

    template <typename type> syncset <type> :: listset :: listset(const class prefix & prefix, const bool & dump) : _prefix(prefix), _size(0), _dump(dump)
    {
    }

    template <typename type> syncset <type> :: listset :: listset(const listset & that) : _prefix(that._prefix), _elements(new type[that._size]), _size(that._size), _dump(that._dump)
    {
        for(size_t i = 0; i < this->_size; i++)
            this->_elements[i] = that._elements[i];
    }

    template <typename type> syncset <type> :: listset :: listset(listset && that) : _prefix(that._prefix), _elements(that._elements), _size(that._size), _dump(that._dump)
    {
        that._size = 0;
    }

    // Destructor

    template <typename type> syncset <type> :: listset :: ~listset()
    {
        if(this->_size)
            delete [] this->_elements;
    }

    // Getters

    template <typename type> const typename syncset <type> :: prefix & syncset <type> :: listset :: prefix() const
    {
        return this->_prefix;
    }

    template <typename type> size_t syncset <type> :: listset :: size() const
    {
        return this->_size;
    }

    template <typename type> const bool & syncset <type> :: listset :: dump() const
    {
        return this->_dump;
    }

    // Methods

    template <typename type> template <typename vtype> void syncset <type> :: listset :: accept(bytewise :: reader <vtype> & reader) const
    {
        reader << (this->_prefix);
        reader << (this->_size);

        for(size_t i = 0; i < this->_size; i++)
            reader << this->_elements[i];

        reader << (this->_dump);
    }

    template <typename type> template <typename vtype> void syncset <type> :: listset :: accept(bytewise :: writer <vtype> & writer)
    {
        writer >> (this->_prefix);
        writer >> (this->_size);

        this->_elements = new type[this->_size];

        for(size_t i = 0; i < this->_size; i++)
            writer >> this->_elements[i];

        writer >> (this->_dump);
    }

    // Operators

    template <typename type> const type & syncset <type> :: listset :: operator [] (const size_t & index) const
    {
        return this->_elements[index];
    }

    template <typename type> bool syncset <type> :: listset :: operator == (const listset & rho) const
    {
        if(this->_size != rho._size)
            return false;

        for(size_t i = 0; i < this->_size; i++)
            if(this->_elements[i] != rho._elements[i])
                return false;

        return true;
    }

    template <typename type> bool syncset <type> :: listset :: operator != (const listset & rho) const
    {
        return !((*this) == rho);
    }

    template <typename type> typename syncset <type> :: listset & syncset <type> :: listset :: operator = (const listset & rho)
    {
        this->~listset();

        this->_prefix = rho._prefix;
        this->_size = rho._size;
        this->_elements = new type[this->_size];

        for(size_t i = 0; i < this->_size; i++)
            this->_elements[i] = rho._elements[i];

        this->_dump = rho._dump;

        return (*this);
    }

    template <typename type> typename syncset <type> :: listset & syncset <type> :: listset :: operator = (listset && rho)
    {
        this->~listset();

        this->_prefix = rho._prefix;
        this->_size = rho._size;
        this->_elements = rho._elements;
        this->_dump = rho._dump;

        rho._size = 0;

        return (*this);
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
            bool collision = false;

            navigator->match([&](empty & empty)
            {
                (*navigator) = empty.fill(element);
            }, [&](single & single)
            {
                if(single.element() == element)
                {
                    collision = true;
                    return;
                }

                class prefix singleprefix(single.label());
                (*navigator) = single.push(singleprefix[navigator.depth()] ? right : left);
            });

            if(collision)
                return;
        }

        navigator--;

        while(true)
        {
            if(navigator.depth() != 0)
                navigator--;
            else
                return;

            navigator->template reinterpret <multiple> ().refresh();
        }
    }

    template <typename type> void syncset <type> :: remove(const type & element)
    {
        prefix prefix(element);
        navigator navigator(prefix, this->_root);

        for(; navigator; navigator++)
        {
            navigator->match([&](single & single)
            {
                if(single.element() == element)
                    (*navigator) = single.empty();
            });
        }

        navigator--;

        while(true)
        {
            if(navigator.depth() != 0)
                navigator--;
            else
                return;

            if(navigator->template reinterpret <multiple> ().pullable())
                (*navigator) = navigator->template reinterpret <multiple> ().pull();
            else
                navigator->template reinterpret <multiple> ().refresh();
        }
    }

    template <typename type> bool syncset <type> :: find(const type & element)
    {
        for(navigator navigator(prefix(element), this->_root); navigator; navigator++)
        {
            optional <bool> found;
            navigator->match([&](single & single)
            {
                if(single.element() == element)
                    found = true;
                else
                    found = false;
            });

            if(found)
                return (*found);
        }

        return false;
    }

    template <typename type> typename syncset <type> :: round syncset <type> :: sync()
    {
        round round;
        round.view.push_back(this->get <false> (prefix()));
        return round;
    }

    template <typename type> typename syncset <type> :: round syncset <type> :: sync(const view & view)
    {
        round round;
        round.view.reserve(2 * view.size());

        size_t elements = 0;
        for(size_t i = 0; i < view.size(); i++)
            view[i].match([&](const listset & listset)
            {
                elements += listset.size();
            });

        round.add.reserve(elements);
        round.remove.reserve(elements);

        for(size_t i = 0; i < view.size(); i++)
        {
            view[i].match([&](const labelset & remote)
            {
                this->get <false> (remote.prefix()).match([&](const labelset & local)
                {
                    if(remote != local)
                    {
                        round.view.push_back(this->get <false> (remote.prefix().left()));
                        round.view.push_back(this->get <false> (remote.prefix().right()));
                    }
                }, [&](const listset & local)
                {
                    round.view.push_back(local);
                });
            }, [&](const listset & remote)
            {
                listset local = this->get <true> (remote.prefix()).template reinterpret <listset> ();
                if(remote != local)
                {
                    hash remotehash = remote.size() ? hash(remote[0]) : hash();
                    hash localhash = local.size() ? hash(local[0]) : hash();

                    size_t i = 0;
                    size_t j = 0;

                    while(i < remote.size() && j < local.size())
                    {
                        if(remotehash < localhash)
                        {
                            round.add.push_back(remote[i++]);
                            if(i < remote.size()) remotehash = hash(remote[i]);
                        }
                        else if(localhash < remotehash)
                        {
                            round.remove.push_back(local[j++]);
                            if(j < local.size()) localhash = hash(local[j]);
                        }
                        else
                        {
                            if(++i < remote.size()) remotehash = hash(remote[i]);
                            if(++j < local.size()) localhash = hash(local[j]);
                        }
                    }

                    while(i < remote.size())
                        round.add.push_back(remote[i++]);

                    while(j < local.size())
                        round.remove.push_back(local[j++]);

                    if(!(remote.dump()))
                        round.view.push_back(local);
                }
            });
        }

        return round;
    }

    // Private methods

    template <typename type> template <bool dump> variant <typename syncset <type> :: labelset, typename syncset <type> :: listset> syncset <type> :: get(const prefix & prefix)
    {
        variant <labelset, listset> response;
        navigator navigator(prefix, this->_root);

        for(; navigator && navigator.depth() <= prefix.bits(); navigator++);
        navigator--;

        navigator->match([&](const multiple & multiple)
        {
            if(dump || (multiple.size() <= settings :: list_threshold))
                response = listset(prefix, multiple, dump);
            else
                response = labelset(prefix, multiple);
        }, [&](const single & single)
        {
            bool match = true;

            for(size_t i = navigator.depth() + 1; i <= prefix.bits(); i++)
                match &= (prefix[i] == single.label()[i]);

            if(match)
                response = listset(prefix, single.element(), dump);
            else
                response = listset(prefix, dump);
        }, [&](const empty &)
        {
            response = listset(prefix, dump);
        });

        return response;
    }

    // Static declarations

    template <typename type> hash syncset <type> :: empty :: _label = hasher().finalize();
};

#endif
