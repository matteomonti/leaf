// Includes

#include "tcp.h"

namespace drop :: acceptors
{
    // sync

    // Constructors

    tcp :: sync :: sync()
    {
        this->_socket.listen();
    }

    tcp :: sync :: sync(const uint16_t & port)
    {
        this->_socket.bind(port);
        this->_socket.listen();
    }

    // Getters

    const class address :: port & tcp :: sync :: port() const
    {
        return this->_socket.port();
    }

    // Methods

    connection tcp :: sync :: accept()
    {
        return connection(this->_socket.accept());
    }

    // async

    // Constructors

    tcp :: async :: async() : _acceptor(), _alive(true), _thread(&async :: run, this)
    {
    }

    tcp :: async :: async(const uint16_t & port) : _acceptor(port), _alive(true), _thread(&async :: run, this)
    {
    }

    // Destructor

    tcp :: async :: ~async()
    {
        this->_alive = false;
        this->wake();
        this->_thread.join();
    }

    // Getters

    const class address :: port & tcp :: async :: port() const
    {
        return this->_acceptor.port();
    }

    // Private methods

    void tcp :: async :: run()
    {
        while(true)
        {
            try
            {
                connection connection = this->_acceptor.accept();

                if(!(this->_alive))
                    break;

                this->emit <class connection> (connection);
            }
            catch(...)
            {
            }
        }
    }

    void tcp :: async :: wake()
    {
        connectors :: tcp :: sync :: connect({(class address :: ip){"127.0.0.1"}, this->_acceptor.port()});
    }
};
