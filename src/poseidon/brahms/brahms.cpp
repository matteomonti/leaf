// Includes

#include "brahms.h"

namespace poseidon
{
    using namespace drop;
    using namespace vine;

    // node

    // Private constructors

    brahms :: node :: node(const vine :: identifier & identifier, brahms & brahms) : vine :: identifier(identifier), _brahms(brahms)
    {
    }

    // Methods

    promise <dial> brahms :: node :: connect() const
    {
        return this->_brahms._dialer.connect(*this);
    }

    // brahms

    // Constructors

    brahms :: brahms(const std :: array <vine :: identifier, settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _view(view), _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
    {
        std :: sort(this->_view.begin(), this->_view.end());
    }

    brahms :: brahms(const class signer & signer, const std :: array <vine :: identifier, settings :: view :: size> & view, typename settings :: dialer & dialer, pool & pool, crontab & crontab, std :: ostream & log) : _signer(signer), _view(view), _dialer(dialer), _pool(pool), _crontab(crontab), log(log)
    {
        std :: sort(this->_view.begin(), this->_view.end());
    }

    // Getters

    const identifier & brahms :: identifier() const
    {
        return this->_signer.publickey();
    }

    signer & brahms :: signer()
    {
        return this->_signer;
    }

    // Methods

    void brahms :: start()
    {
        for(size_t i = 0; i < settings :: view :: size; i++)
        {
            if((i == 0) || (this->_view[i] != this->_view[i - 1]))
            {
                this->update_sample(this->_view[i]);
                this->emit <events :: view :: join> (this->_view[i]);
            }
        }

        this->_dialer.on <dial> ([=](const dial & dial)
        {
            this->serve(this->_pool.bind(dial));
        });

        this->run();
    }

    // Private methods

    void brahms :: update_sample(const vine :: identifier & identifier)
    {
        for(size_t i = 0; i < settings :: sample :: size; i++)
            this->_sample[i].next(identifier);
    }

    void brahms :: update_view(const std :: array <vine :: identifier, settings :: view :: size> & view)
    {
        size_t i = 0;
        size_t j = 0;

        while((i < settings :: view :: size) && (j < settings :: view :: size))
        {
            if(this->_view[i] < view[j])
            {
                this->emit <events :: view :: leave> (this->_view[i]);

                i++;
                while((i < settings :: view :: size) && (this->_view[i] == this->_view[i - 1]))
                    i++;
            }
            else if(this->_view[i] > view[j])
            {
                this->emit <events :: view :: join> (view[j]);

                j++;
                while((j < settings :: view :: size) && (view[j] == view[j - 1]))
                    j++;
            }
            else
            {
                i++;
                while((i < settings :: view :: size) && (this->_view[i] == this->_view[i - 1]))
                    i++;

                j++;
                while((j < settings :: view :: size) && (view[j] == view[j - 1]))
                    j++;
            }
        }

        while(i < settings :: view :: size)
        {
            this->emit <events :: view :: leave> (this->_view[i]);

            i++;
            while((i < settings :: view :: size) && (this->_view[i] == this->_view[i - 1]))
                i++;
        }

        while(j < settings :: view :: size)
        {
            this->emit <events :: view :: join> (view[j]);

            j++;
            while((j < settings :: view :: size) && (this->_view[j] == this->_view[j - 1]))
                j++;
        }
    }

    optional <std :: array <vine :: identifier, brahms :: settings :: view :: size>> brahms :: next_view()
    {
        if((this->_pushslots.size() == 0) || (this->_pushslots.size() > settings :: alpha))
            return null;

        bool empty = true;
        for(size_t i = 0; i < settings :: beta; i++)
            if(this->_pullslots[i].completed)
            {
                empty = false;
                break;
            }

        if(empty)
            return null;

        empty = true;
        for(size_t i = 0; i < settings :: sample :: size; i++)
            if(this->_sample[i].sample())
            {
                empty = false;
                break;
            }

        if(empty)
            return null;

        optional <std :: array <vine :: identifier, settings :: view :: size>> view = def;

        for(size_t i = 0; i < settings :: alpha; i++)
            (*view)[i] = this->_pushslots[randombytes_uniform(this->_pushslots.size())];

        for(size_t i = settings :: alpha; i < settings :: alpha + settings :: beta; i++)
        {
            while(true)
            {
                size_t slot = randombytes_uniform(settings :: beta);
                if(this->_pullslots[slot].completed)
                {
                    while(((*view)[i] = this->_pullslots[slot].view[randombytes_uniform(settings :: view :: size)]) == this->_signer.publickey());
                    break;
                }
            }
        }

        for(size_t i = settings :: alpha + settings :: beta; i < settings :: alpha + settings :: beta + settings :: gamma; i++)
        {
            while(true)
            {
                size_t sample = randombytes_uniform(settings :: sample :: size);
                if(this->_sample[sample].sample())
                {
                    (*view)[i] = *(this->_sample[sample].sample());
                    break;
                }
            }
        }

        std :: sort(view->begin(), view->end());
        return view;
    }

    promise <void> brahms :: pull(vine :: identifier identifier, size_t slot, size_t version)
    {
        try
        {
            pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect(identifier));

            co_await connection.send(false);

            std :: array <vine :: identifier, settings :: view :: size> view;
            for(size_t i = 0; i < settings :: view :: size; i++)
                view[i] = co_await connection.receive <vine :: identifier> ();

            this->_mutex.lock();
            if(this->_version == version)
            {
                this->_pullslots[slot].view = view;
                this->_pullslots[slot].completed = true;
            }
            this->_mutex.unlock();
        }
        catch(...)
        {
        }
    }

    promise <void> brahms :: push(vine :: identifier identifier)
    {
        try
        {
            pool :: connection connection = this->_pool.bind(co_await this->_dialer.connect(identifier));

            co_await connection.send(true);
            co_await connection.send(this->_signer.publickey());
        }
        catch(...)
        {
        }
    }

    promise <void> brahms :: serve(pool :: connection connection)
    {
        try
        {
            bool push = co_await connection.receive <bool> ();

            if(push)
            {
                vine :: identifier identifier = co_await connection.receive <vine :: identifier> ();

                if(this->emit <events :: push :: receive> (identifier))
                {
                    this->_mutex.lock();
                    this->_pushslots.push_back(identifier);
                    this->_mutex.unlock();
                }
            }
            else
            {
                this->_mutex.lock();
                std :: array <vine :: identifier, settings :: view :: size> view = this->_view;
                this->_mutex.unlock();

                for(size_t i = 0; i < settings :: view :: size; i++)
                    co_await connection.send <vine :: identifier> (view[i]);
            }
        }
        catch(...)
        {
        }
    }

    promise <void> brahms :: run()
    {
        co_await this->_crontab.wait(settings :: interval);

        while(true)
        {
            try
            {
                this->_mutex.lock();

                this->_pushslots.clear();
                for(size_t i = 0; i < settings :: alpha; i++)
                {
                    vine :: identifier identifier = this->_view[randombytes_uniform(settings :: view :: size)];

                    if(this->emit <events :: push :: send> (identifier))
                        this->push(identifier);
                }

                this->_version++;
                for(size_t i = 0; i < settings :: beta; i++)
                {
                    this->_pullslots[i].completed = false;

                    vine :: identifier identifier = this->_view[randombytes_uniform(settings :: view :: size)];
                    this->pull(identifier, i, this->_version);
                }

                this->_mutex.unlock();

                co_await this->_crontab.wait(settings :: interval);

                this->_mutex.lock();

                auto view = this->next_view();

                if(view)
                {
                    for(size_t i = 0; i < settings :: view :: size; i++)
                        this->update_sample((*view)[i]);

                    this->update_view(*view);
                }

                this->_mutex.unlock();
            }
            catch(...)
            {
            }
        }
    }
};
