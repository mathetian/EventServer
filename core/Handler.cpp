// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Handler.h"

namespace sealedserver
{

Handler::Handler(EventLoop* loop) : m_loop(loop)
{
    m_status  = 0;
    m_delflag = 0;
}

void Handler::registerRead()
{
    assert(m_loop && m_sock.fd() >= 0);
    m_loop->registerRead(m_sock.fd());
}

void Handler::registerWrite()
{
    assert(m_loop && m_sock.fd() >= 0);
    m_loop->registerWrite(m_sock.fd());
}

void Handler::unRegisterRead()
{
    assert(m_loop && m_sock.fd() >= 0);
    m_loop->unRegisterRead(m_sock.fd());
}

void Handler::unRegisterWrite()
{
    assert(m_loop && m_sock.fd() >= 0);
    m_loop->unRegisterWrite(m_sock.fd());
}

Socket Handler::getSocket()
{
    return m_sock;
}

EventLoop *Handler::getLoop() const
{
    return m_loop;
}

int Handler::getStatus() const
{
    return m_status;
}

int Handler::getDelflag() const
{
    return m_delflag;
}

int Handler::setDelflag()
{
    m_delflag = 1;
}

void Handler::updateStatus(int val)
{
    if(m_delflag == 1) return;

    assert((m_status & val) == 0);
    m_status |= val;
}

void Handler::removeStatus(int val)
{
    if(m_delflag == 1) return;

    assert((m_status & val) != 0);
    m_status ^= val;
}

void Handler::attach()
{
    assert(m_loop && m_sock.fd() >= 0);

    m_loop->attachHandler(m_sock.fd(), this);
}

void Handler::detach()
{
    assert(m_loop && m_sock.fd() >= 0);
    
    m_loop->detachHandler(m_sock.fd(), this);
}

};