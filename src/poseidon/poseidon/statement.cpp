// Includes

#include "statement.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // index

    // Constructors

    index :: index()
    {
    }

    index :: index(const vine :: identifier & identifier, const uint64_t & sequence) : _identifier(identifier), _sequence(sequence)
    {
    }

    // Methods

    const identifier & index :: identifier() const
    {
        return this->_identifier;
    }

    const uint64_t & index :: sequence() const
    {
        return this->_sequence;
    }

    // Operators

    bool index :: operator == (const index & rho) const
    {
        return (this->_identifier == rho._identifier) && (this->_sequence == rho._sequence);
    }

    // statement

    // Signatures

    const hash statement :: signatures :: statement = hash(buffer("poseidon :: statement :: signatures :: statement"));

    // Constructors

    statement :: statement()
    {
    }

    statement :: statement(signer & signer, const uint64_t & sequence, const buffer & value) : _index(signer.publickey(), sequence), _value(value), _signature(signer.sign(signatures :: statement, sequence, value))
    {
    }

    // Getters

    const index & statement :: index() const
    {
        return this->_index;
    }

    const identifier & statement :: identifier() const
    {
        return this->_index.identifier();
    }

    const uint64_t & statement :: sequence() const
    {
        return this->_index.sequence();
    }

    const buffer & statement :: value() const
    {
        return this->_value;
    }

    const signature & statement :: signature() const
    {
        return this->_signature;
    }

    // Methods

    void statement :: verify() const
    {
        verifier verifier(this->_index.identifier());
        verifier.verify(this->_signature, signatures :: statement, this->_index.sequence(), this->_value);
    }

    // Operators

    bool statement :: operator == (const statement & rho) const
    {
        return (this->_index == rho._index) && (this->_value == rho._value);
    }

    bool statement :: operator != (const statement & rho) const
    {
        return !((*this) == rho);
    }
};
