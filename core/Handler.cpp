// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Handler.h"
#include "EventLoop.h"

namespace sealedserver
{

Handler::Handler(EventLoop* loop) : 
    m_loop(loop), m_status(0), m_delflag(false)
{
}

Handler::~Handler()
{
    /// Nothing need to be done
    /// Socket has been closed in other functions
}

void Handler::registerRead()
{
    assert(m_loop && m_sock.status());
    m_loop->registerRead(m_sock);
}

void Handler::registerWrite()
{
    assert(m_loop && m_sock.status());
    m_loop->registerWrite(m_sock);
}

void Handler::unRegisterRead()
{
    assert(m_loop && m_sock.status());
    m_loop->unRegisterRead(m_sock);
}

void Handler::unRegisterWrite()
{
    assert(m_loop && m_sock.status());
    m_loop->unRegisterWrite(m_sock);
}

Socket Handler::getSocket() const
{
    return m_sock;
}

EventLoop *Handler::getLoop() const
{
    return m_loop;
}

EventLoop *Handler::getRandomLoop() const
{
    return m_loop -> getRandomLoop();
}

int Handler::getStatus() const
{
    return m_status;
}

void Handler::updateStatus(int val)
{
    if(m_delflag == true) return;

    assert((m_status & val) == 0);
    m_status |= val;
}

void Handler::removeStatus(int val)
{
    if(m_delflag == true) return;

    assert((m_status & val) != 0);
    m_status ^= val;
}

int Handler::getDelflag() const
{
    return m_delflag;
}

int Handler::setDelflag()
{
    m_delflag = true;
}

void Handler::attach()
{
    assert(m_loop && m_sock.status());
    m_loop->attachHandler(m_sock.fd(), this);
}

void Handler::detach()
{
    assert(m_loop && m_sock.status());
    m_loop->detachHandler(m_sock.fd(), this);
}

void Handler::proceed(int event)
{
    if((event & EPOLLRDHUP) || (event & EPOLLERR) || (event & EPOLLHUP))
    {
        detach();

        INFO << fd << " " << (event&EPOLLRDHUP) << " " << (event&EPOLLERR) << " " << (event&EPOLLHUP);

        /**
        ** get the information for errno
        **/
        int       error = 0;
        socklen_t errlen = sizeof(error);
        
        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen) == 0)
        {
            INFO <<  "error = " << strerror(error);
        }

        onCloseSocket(CLSERR);
    }
    else
    {
        if((event & EPOLLOUT) != 0)
            onSendMsg();

        if(getDelflag() ==  false && (event & EPOLLIN) != 0)
            onReceiveMsg();
    }
}

};