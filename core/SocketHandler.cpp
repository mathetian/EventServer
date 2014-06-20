// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "SocketHandler.h"

namespace sealedserver
{
inline void SocketHandler::attach()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->attachHandler(m_sock.get_fd(), this);
}

inline void SocketHandler::detach()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->detachHandler(m_sock.get_fd(), this);
}

inline void SocketHandler::registerRead()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->registerRead(m_sock.get_fd());
}

inline void SocketHandler::registerWrite()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->registerWrite(m_sock.get_fd());
}

inline void SocketHandler::unRegisterRead()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->unRegisterRead(m_sock.get_fd());
}

inline void SocketHandler::unRegisterWrite()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->unRegisterWrite(m_sock.get_fd());
}

inline int Selector::dispatch()
{
    int num;

    int timeout = 5*1000; //5s

    num = epoll_wait(m_epollfd, m_events, MAX_NEVENTS, timeout);
    INFO << "dispatch: " << num ;
    for(int i = 0; i < num; i++)
    {
        int what = m_events[i].events;
        SocketHandler *handler = (SocketHandler*)m_events[i].data.ptr;
        m_loop->addActive(handler->getSocket().get_fd(), what);
    }

    m_loop->finDel();
}

inline MSGHandler::MSGHandler(EventLoop* loop, Socket sock, int first) : SocketHandler(loop), first(first)
{
    m_sock = sock;
    m_loop->insert(this);
    //onProceed();
}

inline void MSGHandler::onCloseSocket(int st)
{
    if(errno != 0) DEBUG << strerror(errno);
    DEBUG << "onCloseSocket: " << st << " " << m_sock.get_fd();
    errno = 0;

    detach();
    closedSocket();
    m_sock.close();
    m_loop->addDel(this);
}

inline EventLoop* SocketHandler::getLoop2()
{
    int id = rand()%(m_loop->m_pool->getNum());
    INFO << "getLoop2: " << id;
    return m_loop->m_pool->getLoop(id);
}

};