// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include "Log.h"
using namespace utils;

#include "EventLoop.h"
#include "Handler.h"

namespace sealedserver
{

template<class T>
class TCPAcceptor : public Handler
{
    TCPAcceptor& operator=(const TCPAcceptor<T>&acceptor);
    
public:
    TCPAcceptor() : Handler(NULL) { }

    TCPAcceptor(EventLoop* _loop, int localport) : Handler(_loop)
    {
        NetAddress addr = NetAddress(localport);
        m_sock = TCPSocket(&addr);
        onProceed();
    }

    TCPAcceptor(EventLoop* _loop, string ip, int localport) : Handler(_loop)
    {
        NetAddress addr = NetAddress(ip,localport);
        m_sock = TCPSocket(&addr);
        onProceed();
    }

    void onProceed()
    {
        attach();
        registerRead();
        assert(m_sock.fd() >= 0);
        INFO << "TCPAcceptor Initialization" ;
        INFO << m_sock.getsockname() ;
    }

private:
    void onReceiveMsg()
    {
        NetAddress a;
        TCPSocket sock = m_sock.accept(&a);
        DEBUG << "New Connection: " << sock.fd() << " " << sock.getpeername();
        if (sock.fd() >= 0)
        {
            T* t = new T(getLoop2(), sock);
        }
    }

    void onSendMsg() { }
    void onCloseSocket(int st)
    {
        assert(st == 0);
        DEBUG << "close listen socket fd: " << m_sock.fd();

        detach();
        m_sock.close();
    }
};

};

#endif