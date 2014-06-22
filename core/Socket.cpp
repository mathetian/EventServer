// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Socket.h"

namespace sealedserver
{

Socket::Socket(int fd) : fd(-1) { }

Socket::Socket(int family, int type) : 
    m_fd(::socket(family, type, 0))
{
    setUnblocking();

    int optval = 1;

    setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    setStatus();
}

int Socket::fd() const
{
    return m_fd;
}

bool Socket::bindListen(const Address *paddr)
{
    bind(paddr); listen();
    
    return setStatus();
}

bool Socket::connect(const Address *paddr)
{
    if (::connect(fd(), paddr->data(), paddr->length()) != 0)
    {
        if (errno != EINPROGRESS) 
            m_status = Status(strerror(errno));
    }

    errno = 0;
    return status == true ? true : false;
}


bool Socket::bind(const Address *paddr)
{
    ::bind(fd(), paddr->data(), paddr->length());

    return setStatus();
}

bool Socket::listen(int backlog)
{
    ::listen(fd(), backlog);

    return setStatus();
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

            setStatus();
            return Socket();
        }
        else 
            break;
    }

    pa->setAddr(&addr, addrlen);
    
    return Socket(new_fd);
}

void Socket::close()
{
    ::close(m_fd);
}

int Socket::read(void *buf, uint32_t count)
{
    count = ::read(m_fd, buf, count);

    setStatus();

    return count;
}

int Socket::write(const void *buf, int count)
{
    count = ::write(m_fd, buf, count);

    setStatus();

    return count;
}

NetAddress Socket::getpeername()
{
    char buf[sizeof(sockaddr_in) + 1];
    memset(buf, 0, sizeof(buf));

    socklen_t len = sizeof(buf);

    int ret = ::getpeername(fd(), (sockaddr*)&buf, &len);

    setStatus();

    return NetAddress(buf, len);
}

NetAddress Socket::getsockname()
{
    char buf[sizeof(sockaddr_in) + 1];
    memset(buf, 0, sizeof(buf));
    socklen_t len = sizeof(buf);

    int ret = ::getsockname(fd(), (sockaddr*)buf, &len);
    
    setStatus();

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

bool Socket::setStatus()
{
    if(errno != 0)
        m_status = Status(strerror(errno));

    errno = 0;
    return status == true ? true : false;
}

};