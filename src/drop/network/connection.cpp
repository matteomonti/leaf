// Includes

#include "connection.hpp"

namespace drop
{
    // arc

    // Destructor

    connection :: arc :: ~arc()
    {
        this->_socket.visit([](auto & socket)
        {
            if constexpr (!(std :: is_const <std :: remove_reference_t <decltype(socket)>> :: value))
                socket.close();
        });
    }

    // Getters

    int connection :: arc :: descriptor() const
    {
        int descriptor;

        this->_socket.visit([&](auto & socket)
        {
            descriptor = socket.descriptor();
        });

        return descriptor;
    }

    // Setters

    void connection :: arc :: block(const bool & value)
    {
        this->_socket.visit([&](auto & socket)
        {
            if constexpr (!(std :: is_const <std :: remove_reference_t <decltype(socket)>> :: value))
                socket.block(value);
        });
    }

    // Methods

    void connection :: arc :: send(const buffer & message)
    {
        try
        {
            this->_mutex.send.lock();

            this->send_init(message);
            while(!(this->send_step()));

            this->_mutex.send.unlock();
        }
        catch(...)
        {
            this->_mutex.send.unlock();
            std :: rethrow_exception(std :: current_exception());
        }
    }

    buffer connection :: arc :: receive()
    {
        try
        {
            this->_mutex.receive.lock();

            this->receive_init();
            while(!(this->receive_step()));

            this->_mutex.receive.unlock();
        }
        catch(...)
        {
            this->_mutex.receive.unlock();
            std :: rethrow_exception(std :: current_exception());
        }

        return this->receive_yield();
    }

    void connection :: arc :: authenticate(const keyexchanger & exchanger, const class keyexchanger :: publickey & remote)
    {
        class secretbox :: nonce txnonce = secretbox :: nonce :: random();
        this->send(txnonce);

        class secretbox :: nonce rxnonce = this->receive <class secretbox :: nonce> ();

        keyexchanger :: sessionkey session = exchanger.exchange(remote);

        this->_secretchannel.transmit.emplace(session.transmit(), txnonce);
        this->_secretchannel.receive.emplace(session.receive(), rxnonce);
    }

    // Private methods

    void connection :: arc :: send_init(const buffer & message)
    {
        if(this->_secretchannel.transmit)
        {
            buffer ciphertext = this->_secretchannel.transmit->encrypt(message);
            this->_write.buffer = bytewise :: serialize(ciphertext);
        }
        else
            this->_write.buffer = bytewise :: serialize(message);

        this->_write.cursor = 0;
    }

    bool connection :: arc :: send_step()
    {
        bool completed;

        this->_socket.visit([&](auto && socket)
        {
            if constexpr (!(std :: is_const <std :: remove_reference_t <decltype(socket)>> :: value))
            {
                size_t sent = socket.send((const uint8_t *)(this->_write.buffer) + this->_write.cursor, this->_write.buffer.size() - this->_write.cursor);
                this->_write.cursor += sent;

                completed = (this->_write.cursor == this->_write.buffer.size());
            }
        });

        return completed;
    }

    void connection :: arc :: receive_init()
    {
        this->_read = varint :: streamer();
    }

    bool connection :: arc :: receive_step()
    {
        bool completed = false;

        this->_socket.visit([&](auto && socket)
        {
            if constexpr (!(std :: is_const <std :: remove_reference_t <decltype(socket)>> :: value))
            {
                bool loop = false;

                do
                {
                    this->_read.match([&](varint :: streamer & streamer)
                    {
                        if(socket.receive(streamer))
                        {
                            size_t size = streamer.yield();
                            this->_read = buffer :: streamer(size);
                            loop = true;
                        }
                    }, [&](buffer :: streamer & streamer)
                    {
                        completed = socket.receive(streamer);
                        loop = false;
                    });
                }
                while(loop);
            }
        });

        return completed;
    }

    buffer connection :: arc :: receive_yield()
    {
        if(this->_secretchannel.receive)
        {
            buffer ciphertext = this->_read.reinterpret <buffer :: streamer> ().yield();
            return this->_secretchannel.receive->decrypt(ciphertext);
        }
        else
            return this->_read.reinterpret <buffer :: streamer> ().yield();
    }

    // connection

    // Methods

    void connection :: send(const buffer & message) const
    {
        this->_arc->send(message);
    }

    buffer connection :: receive() const
    {
        return this->_arc->receive();
    }

    void connection :: authenticate(const keyexchanger & exchanger, const class keyexchanger :: publickey & remote) const
    {
        this->_arc->authenticate(exchanger, remote);
    }
};
