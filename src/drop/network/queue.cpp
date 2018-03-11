// Includes

#include "queue.h"

namespace drop
{
    // Exceptions

    const char * queue :: kevent_failed :: what() const noexcept
    {
        return "Kevent failed.";
    }

    const char * queue :: epoll_ctl_failed :: what() const noexcept
    {
        return "Epoll_ctl failed.";
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
        if(this->events == EPOLLOUT)
            return write;
        else if(this->events == EPOLLIN)
            return read;
        else
            assert(false);
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

    template <> void queue :: add <queue :: write> (const int & descriptor)
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
            throw kevent_failed();
        #endif

        #ifdef __linux__
        struct epoll_event event
        {
            .data.fd = descriptor,
            .events = EPOLLOUT
        };

        if(epoll_ctl(this->_descriptor, EPOLL_CTL_ADD, descriptor, &event) < 0)
            throw epoll_ctl_failed();
        #endif
    }

    template <> void queue :: add <queue :: read> (const int & descriptor)
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
            throw kevent_failed();
        #endif

        #ifdef __linux__
        struct epoll_event event
        {
            .data.fd = descriptor,
            .events = EPOLLIN
        };

        if(epoll_ctl(this->_descriptor, EPOLL_CTL_ADD, descriptor, &event) < 0)
            throw epoll_ctl_failed();
        #endif
    }

    template <> void queue :: remove <queue :: write> (const int & descriptor)
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
            throw kevent_failed();
        #endif

        #ifdef __linux__
        struct epoll_event event
        {
            .data.fd = descriptor,
            .events = EPOLLOUT
        };

        if(epoll_ctl(this->_descriptor, EPOLL_CTL_DEL, descriptor, &event) < 0)
            throw epoll_ctl_failed();
        #endif
    }

    template <> void queue :: remove <queue :: read> (const int & descriptor)
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
            throw kevent_failed();
        #endif

        #ifdef __linux__
        struct epoll_event event
        {
            .data.fd = descriptor,
            .events = EPOLLIN
        };

        if(epoll_ctl(this->_descriptor, EPOLL_CTL_DEL, descriptor, &event) < 0)
            throw epoll_ctl_failed();
        #endif
    }

    size_t queue :: select()
    {
        #ifdef __APPLE__
        return kevent(this->_descriptor, 0, 0, this->_events, (int) settings :: buffer, nullptr);
        #endif

        #ifdef __linux__
        return epoll_wait(this->_descriptor, this->_events, (int) settings :: buffer, 0);
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

        return kevent(this->_descriptor, 0, 0, this->_events, (int) settings :: buffer, &ktimeout);
        #endif

        #ifdef __linux__
        return epoll_wait(this->_descriptor, this->_events, (int) settings :: buffer, (int) (((const uint64_t &) timeout) / 1000));
        #endif
    }

    // Operators

    const queue :: event & queue :: operator [] (const size_t & index) const
    {
        return this->_events[index];
    }
};
