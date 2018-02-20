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

    buffer ciphertext = alice.encrypt(bob.publickey(), "Attack at dawn!");
    std :: cout << "(" << ciphertext.size() << ") " << ciphertext << std :: endl;

    buffer plaintext = bob.decrypt(alice.publickey(), ciphertext);
    std :: cout << "(" << plaintext.size() << ") " << plaintext << std :: endl;
}
