// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Handler.h"

namespace sealedserver
{

class EventLoop;

class Handler
{
protected:
    EventLoop*  m_loop;
    Socket      m_sock;
    int         m_status;
    int         m_delflag;

public:
    Handler(EventLoop* loop) : m_loop(loop)
    {
        m_status  = 0;
        m_delflag = 0;
    }

public:
    virtual void onReceiveMsg()  = 0;
    virtual void onSendMsg()     = 0;
    virtual void onCloseSocket(int st) = 0;
    virtual void onProceed() = 0;

    void registerRead();
    void registerWrite();
    void unRegisterRead();
    void unRegisterWrite();

    Socket getSocket()
    {
        return m_sock;
    }

    EventLoop *getLoop() const
    {
        return m_loop;
    }

    EventLoop *getLoop2();

    int getStatus() const
    {
        return m_status;
    }

    int setdelflag()
    {
        m_delflag = 1;
    }

    int getdelflag() const
    {
        return m_delflag;
    }

    void updateStatus(int val)
    {
        if(m_delflag == 1) return;

        assert((m_status & val) == 0);
        m_status |= val;
    }

    void removeStatus(int val)
    {
        if(m_delflag == 1) return;

        assert((m_status & val) != 0);
        m_status ^= val;
    }

protected:
    void attach();
    void detach();

    friend class EventLoop;
};

};

namespace sealedserver
{
inline void Handler::attach()
{
    assert(m_loop && m_sock.fd() >= 0);
    m_loop->attachHandler(m_sock.fd(), this);
}

inline void Handler::detach()
{
    assert(m_loop && m_sock.fd() >= 0);
    m_loop->detachHandler(m_sock.fd(), this);
}

inline void Handler::registerRead()
{
    assert(m_loop && m_sock.fd() >= 0);
    m_loop->registerRead(m_sock.fd());
}

inline void Handler::registerWrite()
{
    assert(m_loop && m_sock.fd() >= 0);
    m_loop->registerWrite(m_sock.fd());
}

inline void Handler::unRegisterRead()
{
    assert(m_loop && m_sock.fd() >= 0);
    m_loop->unRegisterRead(m_sock.fd());
}

inline void Handler::unRegisterWrite()
{
    assert(m_loop && m_sock.fd() >= 0);
    m_loop->unRegisterWrite(m_sock.fd());
}

inline MSGHandler::MSGHandler(EventLoop* loop, Socket sock, int first) : Handler(loop), first(first)
{
    m_sock = sock;
    m_loop->insert(this);
    //onProceed();
}

inline void MSGHandler::onCloseSocket(int st)
{
    if(errno != 0) DEBUG << strerror(errno);
    DEBUG << "onCloseSocket: " << st << " " << m_sock.fd();
    errno = 0;

    detach();
    closedSocket();
    m_sock.close();
    m_loop->addDel(this);
}

inline EventLoop* Handler::getLoop2()
{
    int id = rand()%(m_loop->m_pool->getNum());
    INFO << "getLoop2: " << id;
    return m_loop->m_pool->getLoop(id);
}

};