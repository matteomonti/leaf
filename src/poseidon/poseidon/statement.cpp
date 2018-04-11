// Includes

#include "statement.hpp"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Signatures

    const hash statement :: signatures :: statement = hash(buffer("poseidon :: statement :: signatures :: statement"));

    // Constructors

    statement :: statement()
    {
    }

    statement :: statement(signer & signer, const uint64_t & sequence, const buffer & value) : _identifier(signer.publickey()), _sequence(sequence), _value(value), _signature(signer.sign(signatures :: statement, sequence, value))
    {
    }

    // Getters

    const identifier & statement :: identifier() const
    {
        return this->_identifier;
    }

    const uint64_t & statement :: sequence() const
    {
        return this->_sequence;
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
        verifier verifier(this->_identifier);
        verifier.verify(this->_signature, signatures :: statement, this->_sequence, this->_value);
    }
};
