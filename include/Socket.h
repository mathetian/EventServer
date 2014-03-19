#ifndef _SOCKET_H
#define _SOCKET_H

#include "Status.h"
#include "Address.h"

#include <vector>
using namespace std;

#include <sys/un.h>
#include <assert.h>

#include <iostream>
using namespace std;

#include "../utils/Log.h"

class Socket
{
private:
    int    m_fd;
    Status m_stat;

public:
    typedef enum
    {
        none = 0,
        nonblocking = 1,
        acceptor = 2
    } Flags;

    static int sys_socket(int domain, int protocal, int type)
    {
        return ::socket(domain, protocal, type);
    }

public:
    Socket(int fd = -1) : m_fd(fd), m_stat(true) { }

    Socket(int family, int type) : m_fd(sys_socket(family, type, 0)), m_stat(true) {}

    Socket(int family, int type, Address *paddr, Flags f = nonblocking) :
        m_fd(sys_socket(family, type, 0)), m_stat(true)
    {
        DEBUG << "LISTEN or Accept in Socket FD: " << m_fd;
        if (f & nonblocking) set_blocking(false);

        if (paddr)
        {
            if (f & acceptor)
            {
                int optval = 1;
                setsockopt(get_fd(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

                bind(paddr);
                listen();
                if(stat() == false) WARN << "Socket Bind/Listen Error";
                else INFO << "LISTEN Successfully";
            }
            else connect(paddr);
        }
    }

    Socket(Socket const& sock) : m_fd(sock.m_fd), m_stat(sock.m_stat)
    { }

    int get_fd() const
    {
        assert(m_fd >= 0);
        return m_fd;
    }

private:
    void set_status(const Status& s)
    {
        m_stat = s;
    }

    Status bind(const Address *paddr)
    {
        DEBUG << "Bind Address: " << (*(NetAddress*)paddr);
        if (::bind(get_fd(), paddr->data(), paddr->length()) != 0)
        {
            set_status(Status::syserr("bind"));
        }
        return stat();
    }

    Status listen(int backlog = 5)
    {
        if (::listen(get_fd(), backlog) != 0)
        {
            set_status(Status::syserr("listen"));
        }
        return stat();
    }
public:
    Socket accept(Address *pa)
    {
        sockaddr addr;
        socklen_t addrlen = sizeof(addr);
retry:
        int new_fd = ::accept(get_fd(), &addr, &addrlen);
        if (new_fd < 0)
        {
            if (errno == EAGAIN || errno == ECONNABORTED)
                goto retry;

            set_status(Status::syserr("accept"));
            return Socket();
        }
        pa->setAddr(&addr, addrlen);
        INFO << "Accept socket ID: " << new_fd;
        return Socket(new_fd);
    }

    void close()
    {
        assert(get_fd() >= 0);
        ::close(get_fd());
    }

private:
    Status connect(const Address *paddr)
    {
        if (::connect(get_fd(), paddr->data(), paddr->length()) != 0)
        {
            if (errno != EINPROGRESS)
                set_status(Status::syserr("connect"));
        }
        INFO << "connect: " << get_fd() << " " << stat();
        return stat();
    }

    bool set_blocking(bool block)
    {
        int flags = fcntl(get_fd(), F_GETFL);
        assert(flags >= 0);
        flags = block ? flags & ~O_NONBLOCK : flags | O_NONBLOCK;
        assert(fcntl(get_fd(), F_SETFL, flags) >= 0);

        return true;
    }

public:
    NetAddress getpeername()
    {
        char buf[sizeof(sockaddr_in) + 1];
        memset(buf, 0, sizeof(buf));

        socklen_t len = sizeof(buf);

        int ret = ::getpeername(get_fd(), (sockaddr*)&buf, &len);

        assert(ret == 0 && len != sizeof(buf));

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

public:
    int read(void *buf, uint32_t count)
    {
        count = ::read(get_fd(), buf, count);

        if(count < 0 && errno == EAGAIN) WARN << "EWouldBlock Read";
        else if (count < 0) set_status(Status::syserr("Socket::read"));

        return count;
    }

    int write(const void *buf, int count)
    {
        count = ::write(get_fd(), buf, count);
        if(count < 0 && errno == EAGAIN) WARN << "EWouldBlock Read";
        else if(count < 0) set_status(Status::syserr("write"));

        return count;
    }

public:

    Status stat() const
    {
        get_fd();
        return m_stat;
    }

    static pair<Socket, Socket> pipe()
    {
        int fds[2];
        ::pipe(fds);
        return pair<Socket, Socket>(Socket(fds[0]), Socket(fds[1]));
    }

    operator bool()
    {
        return m_fd >= 0 ? true : false;
    }

    string as_string() const
    {
        return to_string(m_fd);
    }
};

TO_STRING(Socket);

class TCPSocket : public Socket
{
public:
    TCPSocket() : Socket() {}
    TCPSocket(Address *paddr, Flags f = none) : Socket(AF_INET, SOCK_STREAM, paddr, f) {  }
    TCPSocket(const Socket& sock) : Socket(sock) {}
};

#endif