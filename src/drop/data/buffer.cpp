// Includes

#include "buffer.h"

namespace drop
{
    // buffer

    // Constructors

    buffer :: buffer() : _size(0), _alloc(0)
    {
    }

    buffer :: buffer(const uint8_t * bytes, const size_t & size) : _bytes(new uint8_t [size + 1]), _size(size), _alloc(size)
    {
        memcpy(this->_bytes, bytes, size);
        this->_bytes[this->_alloc] = '\0';
    }

    buffer :: buffer(const char * bytes, const size_t & size) : buffer((const uint8_t *) bytes, size)
    {
    }

    buffer :: buffer(const char * bytes) : buffer(bytes, strlen(bytes))
    {
    }

    buffer :: buffer(class reference, uint8_t * bytes, const size_t & size) : _bytes(bytes), _size(size), _alloc(0)
    {
    }

    buffer :: buffer(const buffer & that) : buffer(that._bytes, that._size)
    {
    }

    buffer :: buffer(buffer && that) : _bytes(that._bytes), _size(that._size), _alloc(that._alloc)
    {
        that._alloc = 0;
    }

    // Destructor

    buffer :: ~buffer()
    {
        if(this->_alloc)
            delete [] this->_bytes;
    }

    // Getters

    const size_t & buffer :: size() const
    {
        return this->_size;
    }

    // Methods

    void buffer :: alloc(const size_t & size)
    {
        if(this->_alloc < size)
        {
            if(this->_alloc)
              delete [] this->_bytes;

            this->_alloc = size;
            this->_bytes = new uint8_t[this->_alloc + 1];
            this->_bytes[this->_alloc] = '\0';
        }

        this->_size = size;
    }

    // Operators

    uint8_t & buffer :: operator [] (const size_t & index)
    {
        return this->_bytes[index];
    }

    const uint8_t & buffer :: operator [] (const size_t & index) const
    {
        return this->_bytes[index];
    }

    buffer & buffer :: operator = (const buffer & that)
    {
        this->alloc(that._size);
        memcpy(this->_bytes, that._bytes, this->_size);

        return *this;
    }

    buffer & buffer :: operator = (buffer && that)
    {
        if(this->_alloc)
            delete [] this->_bytes;

        this->_size = that._size;
        this->_alloc = that._alloc;
        this->_bytes = that._bytes;

        that._alloc = 0;

        return *this;
    }

    // Casting

    buffer :: operator uint8_t * ()
    {
        return this->_bytes;
    }

    buffer :: operator const uint8_t * () const
    {
        return this->_bytes;
    }

    buffer :: operator char * ()
    {
        return (char *)(this->_bytes);
    }

    buffer :: operator const char * () const
    {
        return (const char *)(this->_bytes);
    }

    // streamer

    // Constructors

    buffer :: streamer :: streamer(const size_t & size) : _cursor(0)
    {
        this->_buffer.alloc(size);
    }

    // Methods

    void buffer :: streamer :: update(const uint8_t * bytes, const size_t & size)
    {
        memcpy(((uint8_t *) (this->_buffer)) + this->_cursor, bytes, size);
        this->_cursor += size;
    }

    size_t buffer :: streamer :: pending()
    {
        return (this->_buffer.size() - this->_cursor);
    }

    buffer && buffer :: streamer :: yield()
    {
        return std :: move(this->_buffer);
    }

    // Ostream integration

    std :: ostream & operator << (std :: ostream & out, const buffer & buffer)
    {
        out << "[";

        for(size_t i = 0; i < buffer.size(); i++)
        {
            if(buffer[i] >= 32 && buffer[i] < 127)
                out << (char)(buffer[i]);
            else
                out << "(" << std :: setfill('0') << std :: setw(2) << std :: hex << (unsigned int)(buffer[i]) << ")" << std :: dec;
        }

        return out << "]";
    }

    // Static members declarations

    class buffer :: reference buffer :: reference;
};
