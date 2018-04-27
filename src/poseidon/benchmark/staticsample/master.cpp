// Includes

#include "master.h"

namespace poseidon :: staticsample
{
    using namespace drop;
    using namespace vine;
    
    // Constructors

    master :: master(const uint16_t & coordinator_port, const uint16_t & directory_port, const size_t & nodes) : _coordinator(coordinator_port, nodes), _directory(directory_port)
    {
    }
};
