// Includes

#include "tcp.hpp"

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

    // callback_interface

    // Destructor

    tcp :: async :: callback_interface :: ~callback_interface()
    {
    }

    // async

    // Constructors

    tcp :: async :: async() : _acceptor(), _callbacks{}, _alive(true), _thread(&async :: run, this)
    {
    }

    tcp :: async :: async(const uint16_t & port) : _acceptor(port), _callbacks{}, _alive(true), _thread(&async :: run, this)
    {
    }

    // Destructor

    tcp :: async :: ~async()
    {
        this->_alive = false;
        this->wake();
        this->_thread.join();

        for(size_t i = 0; this->_callbacks[i]; i++)
            delete this->_callbacks[i];
    }

    // Getters

    const class address :: port & tcp :: async :: port() const
    {
        return this->_acceptor.port();
    }

    // Methods

    void tcp :: async :: reset()
    {
        for(size_t i = 0; this->_callbacks[i]; i++)
        {
            delete this->_callbacks[i];
            this->_callbacks[i] = nullptr;
        }
    }

    // Private methods

    void tcp :: async :: run()
    {
        while(true)
        {
            connection connection = this->_acceptor.accept();

            if(!(this->_alive))
                break;

            for(size_t i = 0; this->_callbacks[i]; i++)
                (*(this->_callbacks[i]))(connection);
        }
    }

    void tcp :: async :: wake()
    {
        connectors :: tcp :: sync :: connect({(class address :: ip){"127.0.0.1"}, this->_acceptor.port()});
    }
};
