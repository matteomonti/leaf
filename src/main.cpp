#include <iostream>
#include <experimental/coroutine>

struct generator
{
    struct promise_type;
    using handle = std :: experimental :: coroutine_handle <promise_type>;

    // Promise type interface

    struct promise_type
    {
        int current_value;

        auto get_return_object() // Quando devo ritornare qualcosa, l'amico istanzia prima questo oggetto, poi mi dice bene, ora wrappaci attorno quello che vuoi.
                                 // Così io prendo questo oggetto, lo ficco in una coroutine_handle, sbatto tutto dentro al generator, e poi lo ritorno.
        {
            return generator{handle::from_promise(*this)}; // Prende questa promise (che contiene il valore corrente, nota bene) e ci wrappa attorno una coroutine_handle.
        }

        auto initial_suspend() // Questa è la differenza tra partire sospeso o partire e arrivare fino al primo coso che interrompe l'esecuzione. So far so good.
        {
            return std::experimental::suspend_never{};
        }

        auto final_suspend() // Che cosa succede se lo metto a false? Apparentemente mi becco un "pointer being freed was not allocated". Non so perché.
        {
            return std::experimental::suspend_always{};
        }

        void unhandled_exception() // Questo chiaramente ammazza il programma, dopo cerchiamo di capire che cosa si potrebbe fare invece.
        {
            std::terminate();
        }

        void return_value(int n)
        {
            std :: cout << "Mi trovo a returnare " << n << std :: endl;
            current_value = n;
        }
    };

    bool move_next()
    {
        return coro ? (coro.resume(), !coro.done()) : false;
    }

    int current_value()
    {
        return coro.promise().current_value;
    }

    generator(generator const&) = delete;
    generator(generator && rhs) : coro(rhs.coro)
    {
        rhs.coro = nullptr;
    }

    ~generator()
    {
        if(coro)
            coro.destroy();
    }

private:

    generator(handle h) : coro(h)
    {
    }

    handle coro;
};

class my_awaitable
{
public:

    std :: experimental :: coroutine_handle <> handle;

    bool await_ready()
    {
        return false;
    }

    void await_suspend(std :: experimental :: coroutine_handle <> handle) // Questo sostanzialmente è un handle di una specie di coroutine che quando la sveglio mi legge
                                                                          // il mio await_resume e lo spara nel risultato di co_await della coroutine che mi ha chiamato.
    {
        this->handle = handle;
    }

    void await_resume()
    {
    }

    void wakeup()
    {
        this->handle.resume();
    }
};

my_awaitable lol;

generator f()
{
    std :: cout << "Entro qui" << std :: endl;
    co_await lol;

    std :: cout << "Riprendo l'esecuzione e sono pronto a tornare" << std :: endl;
    co_return 33;
}

int main()
{
    auto g = f();
    lol.wakeup();

    std :: cout << "The value is " << g.current_value() << std :: endl;
}
