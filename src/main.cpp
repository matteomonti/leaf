#include <iostream>

#include "drop/data/syncset.hpp"

using namespace drop;

int main()
{
    syncset <int> alice;
    syncset <int> bob;

    alice.add(1);
    alice.add(2);
    alice.add(4);
    alice.add(5);
    alice.add(8);
    alice.add(10);

    bob.add(2);
    bob.add(3);
    bob.add(5);
    bob.add(6);
    bob.add(7);
    bob.add(9);

    for(int i = 100; i < 200; i++)
    {
        alice.add(i);
        bob.add(i);
    }

    syncset <int> :: round round;
    buffer message;

    round = alice.sync();
    message = bytewise :: serialize(round.view);

    std :: cout << message << std :: endl << std :: endl;

    round = bob.sync(bytewise :: deserialize <syncset <int> :: view> (message));
    message = bytewise :: serialize(round.view);

    std :: cout << message << std :: endl << std :: endl;

    round = alice.sync(bytewise :: deserialize <syncset <int> :: view> (message));
    message = bytewise :: serialize(round.view);

    std :: cout << message << std :: endl << std :: endl;

    round = bob.sync(bytewise :: deserialize <syncset <int> :: view> (message));
    message = bytewise :: serialize(round.view);

    std :: cout << message << std :: endl << std :: endl;

    round = alice.sync(bytewise :: deserialize <syncset <int> :: view> (message));
    message = bytewise :: serialize(round.view);

    std :: cout << message << std :: endl << std :: endl;

    round = bob.sync(bytewise :: deserialize <syncset <int> :: view> (message));
    message = bytewise :: serialize(round.view);

    std :: cout << message << std :: endl << std :: endl;

    round = alice.sync(bytewise :: deserialize <syncset <int> :: view> (message));
    message = bytewise :: serialize(round.view);

    std :: cout << message << std :: endl << std :: endl;

    round = bob.sync(bytewise :: deserialize <syncset <int> :: view> (message));
    message = bytewise :: serialize(round.view);

    std :: cout << message << std :: endl << std :: endl;
}
