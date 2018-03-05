#include <iostream>

#include "drop/data/syncset.hpp"

using namespace drop;

int main()
{
    syncset <uint64_t> alpha;
    syncset <uint64_t> beta;

    for(size_t i = 0; i < 100000; i++)
    {
        alpha.add(i);
        beta.add(i);
    }

    alpha.remove(44);
    alpha.remove(99);
    alpha.remove(73);

    beta.remove(12);
    beta.remove(13);
    beta.remove(14);
    beta.remove(15);
    beta.remove(16);

    syncset <uint64_t> :: round round = alpha.sync();

    while(true)
    {
        std :: cout << "Round (alpha) (view has " << round.view.size() << " sets):" << std :: endl;

        for(size_t i = 0; i < round.add.size(); i++)
            std :: cout << " + " << round.add[i] << std :: endl;

        for(size_t i = 0; i < round.remove.size(); i++)
            std :: cout << " - " << round.remove[i] << std :: endl;

        if(round.view.size() == 0)
            break;

        round = beta.sync(round.view);

        std :: cout << "Round (beta) (view has " << round.view.size() << " sets):" << std :: endl;

        for(size_t i = 0; i < round.add.size(); i++)
            std :: cout << " + " << round.add[i] << std :: endl;

        for(size_t i = 0; i < round.remove.size(); i++)
            std :: cout << " - " << round.remove[i] << std :: endl;

        if(round.view.size() == 0)
            break;

        round = alpha.sync(round.view);
    }
}
