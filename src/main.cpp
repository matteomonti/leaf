#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "drop/crypto/signature.hpp"

using namespace drop;

int main()
{
    signer my_signer;
    signature my_signature = my_signer.sign(1, 2, buffer("Hello World!"), 3);

    std :: cout << my_signer.publickey() << std :: endl;
    std :: cout << my_signature << std :: endl;

    verifier my_verifier(my_signer.publickey());
    my_verifier.verify(my_signature, 1, 2, buffer("Hello World!"), 3);

    std :: cout << "Verification successful!" << std :: endl;
}
