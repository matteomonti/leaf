// Includes

#include "queue.h"

namespace drop
{
    // Exceptions

    const char * queue :: exceptions :: kevent_failed :: what() const noexcept
    {
        return "Kevent failed.";
    }

    const char * queue :: exceptions :: epoll_ctl_failed :: what() const noexcept
    {
        return "Epoll_ctl failed.";
    }

    const char * queue :: exceptions :: epoll_wait_failed :: what() const noexcept
    {
        return "Epoll_wait failed.";
    }

    // event

    // Getters

    int queue :: event :: descriptor() const
    {
        #ifdef __APPLE__
        return this->ident;
        #endif

        #ifdef __linux__
        return this->data.fd;
        #endif
    }

    queue :: type queue :: event :: type() const
    {
        #ifdef __APPLE__
        if(this->filter == EVFILT_WRITE)
            return write;
        else if(this->filter == EVFILT_READ)
            return read;
        else
            assert(false);
        #endif

        #ifdef __linux__
        if(this->events & EPOLLOUT)
            return write;
        else if(this->events & EPOLLIN)
            return read;
        else
            assert(false);
        #endif
    }

    bool queue :: event :: error() const
    {
        #ifdef __APPLE__
        return (this->flags & EV_ERROR);
        #endif

        #ifdef __linux__
        return (this->events & (EPOLLERR | EPOLLUP));
        #endif
    }

    // queue

    // Constructors

    queue :: queue()
    {
        #ifdef __APPLE__
        this->_descriptor = kqueue();
        #endif

        #ifdef __linux__
        this->_descriptor = epoll_create1(0);
        #endif
    }

    // Destructor

    queue :: ~queue()
    {
        close(this->_descriptor);
    }

    // Methods

    void queue :: add(const int & descriptor, const type & filter)
    {
        if(filter == write)
        {
            #ifdef __APPLE__
            struct kevent event
            {
                .ident = (uintptr_t) descriptor,
                .flags = (uint16_t) EV_ADD,
                .filter = EVFILT_WRITE,
                .fflags = 0,
                .data = 0,
                .udata = nullptr
            };

            if(kevent(this->_descriptor, &event, 1, 0, 0, 0) < 0)
                throw exceptions :: kevent_failed();
            #endif

            #ifdef __linux__
            struct epoll_event event
            {
                .data.fd = descriptor,
                .events = EPOLLOUT
            };

            if(epoll_ctl(this->_descriptor, EPOLL_CTL_ADD, descriptor, &event) < 0)
                throw exceptions :: epoll_ctl_failed();
            #endif
        }
        else
        {
            #ifdef __APPLE__
            struct kevent event
            {
                .ident = (uintptr_t) descriptor,
                .flags = (uint16_t) EV_ADD,
                .filter = EVFILT_READ,
                .fflags = 0,
                .data = 0,
                .udata = nullptr
            };

            if(kevent(this->_descriptor, &event, 1, 0, 0, 0) < 0)
                throw exceptions :: kevent_failed();
            #endif

            #ifdef __linux__
            struct epoll_event event
            {
                .data.fd = descriptor,
                .events = EPOLLIN
            };

            if(epoll_ctl(this->_descriptor, EPOLL_CTL_ADD, descriptor, &event) < 0)
                throw exceptions :: epoll_ctl_failed();
            #endif
        }
    }

    void queue :: remove(const int & descriptor, const type & filter)
    {
        if(filter == write)
        {
            #ifdef __APPLE__
            struct kevent event
            {
                .ident = (uintptr_t) descriptor,
                .flags = (uint16_t) EV_DELETE,
                .filter = EVFILT_WRITE,
                .fflags = 0,
                .data = 0,
                .udata = nullptr
            };

            if(kevent(this->_descriptor, &event, 1, 0, 0, 0) < 0)
                throw exceptions :: kevent_failed();
            #endif

            #ifdef __linux__
            struct epoll_event event
            {
                .data.fd = descriptor,
                .events = EPOLLOUT
            };

            if(epoll_ctl(this->_descriptor, EPOLL_CTL_DEL, descriptor, &event) < 0)
                throw exceptions :: epoll_ctl_failed();
            #endif
        }
        else
        {
            #ifdef __APPLE__
            struct kevent event
            {
                .ident = (uintptr_t) descriptor,
                .flags = (uint16_t) EV_DELETE,
                .filter = EVFILT_READ,
                .fflags = 0,
                .data = 0,
                .udata = nullptr
            };

            if(kevent(this->_descriptor, &event, 1, 0, 0, 0) < 0)
                throw exceptions :: kevent_failed();
            #endif

            #ifdef __linux__
            struct epoll_event event
            {
                .data.fd = descriptor,
                .events = EPOLLIN
            };

            if(epoll_ctl(this->_descriptor, EPOLL_CTL_DEL, descriptor, &event) < 0)
                throw exceptions :: epoll_ctl_failed();
            #endif
        }
    }

    size_t queue :: select()
    {
        #ifdef __APPLE__
        int count = kevent(this->_descriptor, 0, 0, this->_events, (int) settings :: buffer, nullptr);

        if(count < 0)
            throw exceptions :: kevent_failed();
        else
            return count;
        #endif

        #ifdef __linux__
        int count = epoll_wait(this->_descriptor, this->_events, (int) settings :: buffer, 0);

        if(count < 0)
            throw exceptions :: epoll_wait_failed();
        else
            return count;
        #endif
    }

    size_t queue :: select(const interval & timeout)
    {
        #ifdef __APPLE__
        struct timespec ktimeout
        {
            .tv_sec = (__darwin_time_t) (((const uint64_t &) timeout) / 1000000),
            .tv_nsec = (__darwin_time_t) (((const uint64_t &) timeout) % 1000000) * 1000
        };

        int count = kevent(this->_descriptor, 0, 0, this->_events, (int) settings :: buffer, &ktimeout);

        if(count < 0)
            throw exceptions :: kevent_failed();
        else
            return count;
        #endif

        #ifdef __linux__
        int count = epoll_wait(this->_descriptor, this->_events, (int) settings :: buffer, (int) (((const uint64_t &) timeout) / 1000));

        if(count < 0)
            throw exceptions :: epoll_wait_failed();
        else
            return count;
        #endif
    }

    // Operators

    const queue :: event & queue :: operator [] (const size_t & index) const
    {
        return this->_events[index];
    }
};
