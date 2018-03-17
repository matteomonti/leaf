#include <iostream>
#include <unordered_map>

#include "drop/bytewise/bytewise.hpp"
#include "drop/crypto/shorthash.hpp"

using namespace drop;

int main()
{
    std :: unordered_map <buffer, buffer, shorthash> map;
    map[buffer("Hello World!")] = buffer("Does it work?");
    std :: cout << map[buffer("Hello World!")] << std :: endl;
}
