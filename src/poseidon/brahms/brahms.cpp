// Includes

#include "brahms.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // Constructors

    brahms :: brahms(const signer & signer, const view & view, dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _signer(signer), _view(view), _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
    {
        for(size_t index = 0; index < settings :: sample :: size; index++)
            this->_sample[index].init(this->_view);
    }

    // Methods

    void brahms :: start()
    {
        this->_dialer.on <0> ([=](const dial & dial)
        {
            this->serve(this->_pool.bind(dial), dial.identifier());
        });

        this->run();
    }

    // Private methods

    promise <void> brahms :: push(identifier identifier)
    {
        try
        {
            pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <0> (identifier));
            co_await connection.send(true);
        }
        catch(...)
        {
        }
    }

    void brahms :: push()
    {
        for(size_t index = 0; index < settings :: brahms :: alpha; index++)
            this->push(this->_view[randombytes_uniform(settings :: view :: size)]);
    }

    promise <view> brahms :: pull(identifier identifier)
    {
        pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect <0> (identifier));
        co_await connection.send(false);
        view view = co_await connection.receive <poseidon :: view> ();

        co_return view;
    }

    auto brahms :: pull()
    {
        std :: array <promise <view>, settings :: brahms :: beta> promises;

        for(size_t index = 0; index < settings :: brahms :: beta; index++)
            promises[index] = this->pull(this->_view[randombytes_uniform(settings :: view :: size)]);

        return collect(promises, until(this->_crontab.wait(settings :: brahms :: interval)));
    }

    promise <void> brahms :: serve(pool :: connection connection, identifier identifier)
    {
        try
        {
            bool push = co_await(connection.receive <bool> ());

            if(push && this->emit <poseidon :: push> (identifier))
            {
                this->_mutex.lock();
                this->_pushbuffer.push_back(identifier);
                this->_mutex.unlock();
            }
            else
            {
                this->_mutex.lock();
                promise <void> send = connection.send(this->_view);
                this->_mutex.unlock();

                co_await send;
            }
        }
        catch(...)
        {
        }
    }

    promise <void> brahms :: run()
    {
        while(true)
        {
            this->push();
            auto collector = co_await this->pull();

            this->_mutex.lock();

            bool pullsuccess = false;
            for(size_t index = 0; index < settings :: brahms :: beta; index++)
                try
                {
                    collector.get <0> (index);
                    pullsuccess = true;
                    break;
                }
                catch(...) {}

            if(this->_pushbuffer.size() <= settings :: brahms :: alpha && this->_pushbuffer.size() > 0 && pullsuccess)
            {
                // Update the sample

                this->update_sample(this->_pushbuffer);
                for(size_t index = 0; index < settings :: brahms :: beta; index++)
                    try
                    {
                        this->update_sample(collector.get <0> (index));
                    }
                    catch(...) {}

                // Compute the new view

                view newview;

                for(size_t index = 0; index < settings :: brahms :: alpha; index++)
                    newview[index] = this->_pushbuffer[randombytes_uniform(this->_pushbuffer.size())];

                for(size_t index = settings :: brahms :: alpha; index < settings :: brahms :: alpha + settings :: brahms :: beta; index++)
                {
                    while(true)
                    {
                        size_t view = randombytes_uniform(settings :: brahms :: beta);
                        size_t element = randombytes_uniform(settings :: view :: size);

                        try
                        {
                            newview[index] = collector.get <0> (view)[element];
                        }
                        catch(...) {}
                    }
                }

                for(size_t index = settings :: brahms :: alpha + settings :: brahms :: beta; index < settings :: brahms :: alpha + settings :: brahms :: beta + settings :: brahms :: gamma; index++)
                    newview[index] = this->_sample[randombytes_uniform(settings :: sample :: size)].sample();

                this->_view = newview;
            }

            this->_pushbuffer.clear();
            this->_mutex.unlock();
        }
    }

    void brahms :: update_sample(const view & view)
    {
        for(const identifier & identifier : view)
            for(sampler & sampler : this->_sample)
                sampler.next(identifier);
    }

    void brahms :: update_sample(const std :: vector <identifier> & buffer)
    {
        for(const identifier & identifier : buffer)
            for(sampler & sampler : this->_sample)
                sampler.next(identifier);
    }

    void brahms :: reset_sampler(const size_t & index)
    {
        this->_sample[index].init(this->_view);
    }
};
