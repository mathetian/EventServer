// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include "Socket.h"

namespace sealedserver
{

class EventLoop;

class Handler
{
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

protected:
    EventLoop*  m_loop;
    Socket      m_sock;
    int         m_status;
    int         m_delflag;
};

};

#endif