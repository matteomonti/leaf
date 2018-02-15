#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/data/buffer.hpp"
#include "drop/crypto/secretbox.hpp"

using namespace drop;

int main()
{
    secretbox tx;
    secretbox rx(tx.key(), tx.nonce());

    buffer plaintext = "Hello World!";
    buffer ciphertext = tx.encrypt(plaintext);

    std :: cout << "(" << plaintext.size() << ") " << plaintext << std :: endl;
    std :: cout << "(" << ciphertext.size() << ") " << ciphertext << std :: endl;

    plaintext = rx.decrypt(ciphertext);

    std :: cout << "(" << plaintext.size() << ") " << plaintext << std :: endl;

    std :: cout << bytewise :: serialize(1, 2, 3, 4) << std :: endl;
    std :: cout << bytewise :: deserialize <int> (bytewise :: serialize(1)) << std :: endl;
    auto x = bytewise :: deserialize <int, int> (bytewise :: serialize(1, 2));
    std :: cout << std :: get <0> (x) << std :: endl;
    std :: cout << std :: get <1> (x) << std :: endl;
}
