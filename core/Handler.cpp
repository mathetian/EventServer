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

void Handler::registerRead()
{
    assert(m_loop && m_sock.status());
    
    m_loop->registerRead(m_sock.fd());
}

void Handler::registerWrite()
{
    if(m_sock.status() == false)
    {
        INFO << m_sock.status();
    }

    assert(m_loop && m_sock.status());
    
    m_loop->registerWrite(m_sock.fd());
}

void Handler::unRegisterRead()
{
    assert(m_loop && m_sock.status());
    
    m_loop->unRegisterRead(m_sock.fd());
}

void Handler::unRegisterWrite()
{
    assert(m_loop && m_sock.status());
   
    m_loop->unRegisterWrite(m_sock.fd());
}

/// Default behaviors(only be used in Acceptor)
void Handler::onCloseEvent(ClsMtd st)
{
    detach(); m_sock.close(); m_loop -> addClosed(this);
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

bool Handler::getDelflag() const
{
    return m_delflag;
}

bool Handler::setDelflag()
{
    m_delflag = true;
}

void Handler::proceed(int event)
{
    if((event & EPOLLRDHUP) || (event & EPOLLERR) || (event & EPOLLHUP))
    {
        DEBUG << "Proceed: " << m_sock.fd() << " " << (event&EPOLLRDHUP) << " " << (event&EPOLLERR) << " " << (event&EPOLLHUP);
        onCloseEvent(CLSERR);
    }
    else
    {
        if((event & EPOLLOUT) != 0)
            onSendEvent();

        if(getDelflag() ==  false && (event & EPOLLIN) != 0)
            onReceiveEvent();
    }
}

};