#include <iostream>

#include "drop/data/syncset.hpp"

using namespace drop;

int main()
{
    syncset <uint64_t> my_syncset;

    for(size_t i = 0; i < 100000; i++)
        my_syncset.add(i);

    syncset <uint64_t> :: prefix my_prefix(1255433, 5);
    std :: cout << "Target prefix:" << hash(uint64_t(1255433)) << std :: endl;

    auto set = my_syncset.get <true> (my_prefix);
    set.match([](const syncset <uint64_t> :: listset & listset)
    {
        std :: cout << "Listset with " << listset.size() << " elements." << std :: endl;
        for(size_t i = 0; i < listset.size(); i++)
            std :: cout << hash(listset[i]) << std :: endl;
    }, [](const syncset <uint64_t> :: labelset & labelset)
    {
        std :: cout << "Labelset with label " << labelset.label() << std :: endl;
    });
}
