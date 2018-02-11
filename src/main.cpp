#include <iostream>
#include <iomanip>

#include "drop/bytewise/bytewise.hpp"
#include "drop/data/buffer.hpp"

using namespace drop;

struct sizer
{
    size_t size;
    sizer() : size(0)
    {
    }

    void update(const uint8_t *, const size_t & chunk)
    {
        this->size += chunk;
    }
};

struct reader
{
    const uint8_t * bytes;
    size_t cursor;

    reader(const size_t & size) : bytes(new uint8_t[size]), cursor(0)
    {
    }

    void update(const uint8_t * data, const size_t & size)
    {
        memcpy((char *) (this->bytes + this->cursor), (char *) data, size);
        this->cursor += size;
    }
};

struct writer
{
    const uint8_t * bytes;
    size_t cursor;

    writer(const uint8_t * bytes) : bytes(bytes), cursor(0)
    {
    }

    const uint8_t * pop(const size_t & size)
    {
        const uint8_t * res = (this->bytes + this->cursor);
        this->cursor += size;
        return res;
    }
};

struct acceptor
{
    uint32_t i;
    uint64_t j;
    buffer b;

    template <typename vtype> void accept(bytewise :: reader <vtype> & visitor) const
    {
        visitor << this->i << this->j << this->b;
    }

    template <typename vtype> void accept(bytewise :: writer <vtype> & visitor)
    {
        visitor >> this->i >> this->j >> this->b;
    }
};

int main()
{
    acceptor my_acceptor{.i = 99, .j = 101, .b = "Hello World!"};

    sizer my_sizer;
    bytewise :: read(my_sizer, uint32_t(64), uint64_t(12), my_acceptor);

    std :: cout << my_sizer.size << std :: endl << std :: endl;

    reader my_reader(my_sizer.size);
    bytewise :: read(my_reader, uint32_t(64), uint64_t(12), my_acceptor);

    uint32_t x;
    uint64_t y;
    acceptor my_other_acceptor;

    writer my_writer(my_reader.bytes);
    bytewise :: write(my_writer, x, y, my_other_acceptor);

    std :: cout << x << std :: endl;
    std :: cout << y << std :: endl;
    std :: cout << my_other_acceptor.i << std :: endl;
    std :: cout << my_other_acceptor.j << std :: endl;
    std :: cout << my_other_acceptor.b << std :: endl;
}
