#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/crypto/keyexchanger.h"

using namespace drop;

int main()
{
    keyexchanger alice;
    keyexchanger bob;

    auto alicekey = alice.exchange(bob.publickey());
    auto bobkey = bob.exchange(alice.publickey());

    std :: cout << alicekey.transmit() << " - " << bobkey.receive() << std :: endl;
    std :: cout << alicekey.receive() << " - " << bobkey.transmit() << std :: endl;
}
