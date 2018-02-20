#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/data/buffer.hpp"
#include "drop/crypto/secretbox.hpp"
#include "drop/crypto/hash.hpp"

using namespace drop;

int main()
{
    secretbox tx;
    secretbox rx(tx.key(), tx.nonce());

    std :: cout << rx.decrypt <int> (tx.encrypt(33)) << std :: endl << std :: endl;

    auto plaintext = rx.decrypt <int, int, buffer, hash> (tx.encrypt(44, 92, buffer("Hello World!"), hash(buffer("Hash me baby one more time!"))));
    std :: cout << std :: get <0> (plaintext) << ", " << std :: get <1> (plaintext) << ", " << std :: get <2> (plaintext) << ", " << std :: get <3> (plaintext) << std :: endl;
}
