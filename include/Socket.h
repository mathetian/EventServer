#ifndef _SOCKET_H
#define _SOCKET_H

#include <sys/un.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

#include "../utils/Log.h"
using namespace utils;

#include "Address.h"

#define CLSSIG 0
#define CLSEVT 1
#define CLSEOF 2
#define CLSERR 3
#define CLSWRR 4
#define CLSMAN 5
#define CLSHTP 6

class Socket
{
private:
    int    m_fd;

public:
    Socket(int fd = -1) : m_fd(fd) { }

    Socket(int family, int type) :
        m_fd(::socket(family, type, 0))
    {
        set_blocking(false);
        int optval = 1;
        setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
        assert(m_fd >= 0);
    }

    Socket(Socket const& sock) : m_fd(sock.m_fd) { }

    int get_fd() const
    {
        return m_fd;
    }

    bool bindlisten(const Address *paddr)
    {
        bind(paddr);
        listen();
        if(errno != 0) return false;
        return true;
    }

    bool cliConnect(const Address *paddr)
    {
        connect(paddr);
        if(errno != 0)
            return false;
        return true;
    }

public:
    bool bind(const Address *paddr)
    {
        if (::bind(get_fd(), paddr->data(), paddr->length()) != 0)
            return false;
        return true;
    }

    bool listen(int backlog = 5)
    {
        if (::listen(get_fd(), backlog) != 0)
            return false;
        return true;
    }

    Socket accept(Address *pa)
    {
        sockaddr addr;
        socklen_t addrlen = sizeof(addr);
        int new_fd;
        while(true)
        {
            new_fd = ::accept(get_fd(), &addr, &addrlen);
            if (new_fd < 0)
            {
                if (errno == EAGAIN || errno == ECONNABORTED)
                    continue;
                return Socket();
            }
            else break;
        }

        pa->setAddr(&addr, addrlen);
        return Socket(new_fd);
    }


public:
    void close()
    {
        ::close(m_fd);
    }

    int read(void *buf, uint32_t count)
    {
        count = ::read(m_fd, buf, count);

        return count;
    }

    int write(const void *buf, int count)
    {
        count = ::write(m_fd, buf, count);

        return count;
    }

    NetAddress getpeername()
    {
        char buf[sizeof(sockaddr_in) + 1];
        memset(buf, 0, sizeof(buf));

        socklen_t len = sizeof(buf);

        int ret = ::getpeername(get_fd(), (sockaddr*)&buf, &len);

        return NetAddress(buf, len);
    }

    NetAddress getsockname()
    {
        char buf[sizeof(sockaddr_in) + 1];
        memset(buf, 0, sizeof(buf));
        socklen_t len = sizeof(buf);

        int ret = ::getsockname(get_fd(), (sockaddr*)buf, &len);

        assert(ret == 0 && len != sizeof(buf));

        return NetAddress(buf, len);
    }

    static pair<Socket, Socket> pipe() 
    {
        int fds[2]; ::pipe(fds);
        return pair<Socket, Socket>(Socket(fds[0]), Socket(fds[1]));
    }

private:
    bool connect(const Address *paddr)
    {
        if (::connect(get_fd(), paddr->data(), paddr->length()) != 0)
        {
            if (errno != EINPROGRESS) return false;
            return true;
        }
        return true;
    }

    bool set_blocking(bool block)
    {
        int flags = fcntl(get_fd(), F_GETFL);
        flags = block ? flags & ~O_NONBLOCK : flags | O_NONBLOCK;
        assert(fcntl(get_fd(), F_SETFL, flags) >= 0);

        return true;
    }
};

class TCPSocket : public Socket
{
public:
    TCPSocket() : Socket() {}
    TCPSocket(Address *paddr) : Socket(AF_INET, SOCK_STREAM)
    {
        bindlisten(paddr);
    }
    TCPSocket(const Socket& sock) : Socket(sock) {}
};

#endif