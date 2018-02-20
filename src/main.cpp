#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/crypto/box.hpp"
#include "drop/bytewise/bytewise.hpp"

using namespace drop;

int main()
{
    box alice;
    box bob;

    buffer ciphertext = alice.encrypt(bob.publickey(), 1, 2, buffer("Attack at dawn!"), 3);
    std :: cout << "(" << ciphertext.size() << ") " << ciphertext << std :: endl;

    auto plaintext = bob.decrypt <int, int, buffer, int> (alice.publickey(), ciphertext);

    std :: cout << std :: get <0> (plaintext) << std :: endl;
    std :: cout << std :: get <1> (plaintext) << std :: endl;
    std :: cout << std :: get <2> (plaintext) << std :: endl;
    std :: cout << std :: get <3> (plaintext) << std :: endl;
}
