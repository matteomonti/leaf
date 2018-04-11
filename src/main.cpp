#include <iostream>

// Includes

#include "poseidon/poseidon/statement.hpp"

using namespace drop;
using namespace vine;
using namespace poseidon;

int main()
{
    signer alice;
    statement first_statement(alice, 0, "I like apples!");
    first_statement.verify();

    buffer serialized = bytewise :: serialize(first_statement);
    statement deserialized = bytewise :: deserialize <statement> (serialized);

    std :: cout << deserialized.identifier() << std :: endl;
    std :: cout << deserialized.sequence() << std :: endl;
    std :: cout << deserialized.value() << std :: endl;
    std :: cout << deserialized.signature() << std :: endl;

    deserialized.verify();

    std :: cout << "Everything is fine!" << std :: endl;
}
