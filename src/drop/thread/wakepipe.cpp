// Includes

#include "wakepipe.h"

namespace drop
{
    // Constructors

    wakepipe :: wakepipe()
    {
        int wake[2];
        pipe(wake);

        this->_read = wake[0];
        this->_write = wake[1];

        fcntl(this->_read, F_SETFL, O_NONBLOCK);
    }

    // Destructor

    wakepipe :: ~wakepipe()
    {
        close(this->_read);
        close(this->_write);
    }

    // Methods

    void wakepipe :: wake()
    {
        char buffer = '\0';
        write(this->_write, &buffer, 1);
    }

    void wakepipe :: flush()
    {
        char buffer;
        while(read(this->_read, &buffer, 1) >= 0);
    }

    // Casting

    wakepipe :: operator const int & () const
    {
        return this->_read;
    }
};
