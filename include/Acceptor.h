// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include "Log.h"
using namespace utils;

#include "Handler.h"
#include "EventLoop.h"

namespace sealedserver
{

/**
** Users shouldn't override this class
**/
template<class T>
class TCPAcceptor : public Handler
{    
public:
    /// Constructor
    TCPAcceptor(EventLoop* _loop, int localport) : Handler(_loop)
    {
        NetAddress addr = NetAddress(localport);
        
        m_sock = TCPSocket(&addr);
        
        attach(); registerRead();
        assert(m_sock.status());

        INFO << "TCPAcceptor Initialization: " << m_sock.getsockname() ;
    }

    /// Destructor
    virtual ~TCPAcceptor()
    {
    }

private:
    void onReceivEvent()
    {
        NetAddress a;
        TCPSocket sock = m_sock.accept(&a);
        
        DEBUG << "New Connection: " << sock.fd() << " " << sock.getpeername();
        
        if (sock.status() == true)
        {
            T* t = new T(getRandomLoop(), sock);
        }
    }

    void onSendEvent() { }

    void onCloseEvent(ClsMtd st)
    {
        DEBUG << "close listen socket fd: " << m_sock.fd() << " " << st;

        detach(); m_sock.close();
    }
};

};

#endif