// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Socket.h"

namespace sealedserver
{

Socket::Socket(int fd) : m_fd(fd) { }

Socket::Socket(int family, int type) : 
    m_fd(::socket(family, type, 0))
{
    setUnblocking();

    int optval = 1;

    setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    
    assert(m_fd >= 0);
}

int Socket::fd() const
{
    return m_fd;
}

bool Socket::bindListen(const Address *paddr)
{
    bind(paddr); listen();
    
    if(errno != 0) 
        return false;
    return true;
}

bool Socket::clientConnect(const Address *paddr)
{
    connect(paddr);
    
    if(errno != 0)
        return false;
    return true;
}

bool Socket::bind(const Address *paddr)
{
    if (::bind(fd(), paddr->data(), paddr->length()) != 0)
        return false;
    
    return true;
}

bool Socket::listen(int backlog)
{
    if (::listen(fd(), backlog) != 0)
        return false;

    return true;
}

Socket Socket::accept(Address *pa)
{
    sockaddr addr;
    socklen_t addrlen = sizeof(addr);
    int new_fd;
    while(true)
    {
        new_fd = ::accept(fd(), &addr, &addrlen);
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

bool Socket::connect(const Address *paddr)
{
    if (::connect(fd(), paddr->data(), paddr->length()) != 0)
    {
        if (errno != EINPROGRESS) return false;
        return true;
    }
    return true;
}

void Socket::close()
{
    ::close(m_fd);
}

int Socket::read(void *buf, uint32_t count)
{
    count = ::read(m_fd, buf, count);

    return count;
}

int Socket::write(const void *buf, int count)
{
    count = ::write(m_fd, buf, count);

    return count;
}

NetAddress Socket::getpeername()
{
    char buf[sizeof(sockaddr_in) + 1];
    memset(buf, 0, sizeof(buf));

    socklen_t len = sizeof(buf);

    int ret = ::getpeername(fd(), (sockaddr*)&buf, &len);

    return NetAddress(buf, len);
}

NetAddress Socket::getsockname()
{
    char buf[sizeof(sockaddr_in) + 1];
    memset(buf, 0, sizeof(buf));
    socklen_t len = sizeof(buf);

    int ret = ::getsockname(fd(), (sockaddr*)buf, &len);

    assert(ret == 0 && len != sizeof(buf));

    return NetAddress(buf, len);
}

pair<Socket, Socket> Socket::pipe()
{
    int fds[2];
    ::pipe(fds);
    return pair<Socket, Socket>(Socket(fds[0]), Socket(fds[1]));
}

bool Socket::setUnblocking()
{
    int flags = fcntl(fd(), F_GETFL);
    flags = flags | O_NONBLOCK;
    assert(fcntl(fd(), F_SETFL, flags) >= 0);

    return true;
}

};