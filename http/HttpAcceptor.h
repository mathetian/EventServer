// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _HTTP_ACC_H
#define _HTTP_ACC_H

#include "Acceptor.h"
using namespace eventserver;

namespace http
{

class HttpServer;

/**
** Users shouldn't override this class
**/
template<class T>
class HttpAcceptor : public Handler
{
public:
    /// Constructor
    HttpAcceptor(HttpServer *server, EventLoop* loop, int localport) : Handler(loop), server_(server)
    {
        NetAddress addr = NetAddress(localport);

        m_sock = TCPSocket(&addr);

        attach();
        registerRead();

        INFO << "HttpAcceptor Initialization: " << m_sock.getsockname();

        assert(m_sock.status());
    }

    /// Destructor
    virtual ~HttpAcceptor()
    {
    }

private:
    virtual void onReceiveEvent()
    {
        NetAddress a;
        Socket sock = m_sock.accept(&a);

        DEBUG << "New Connection, through socket(local): " << sock.fd();
        DEBUG << "Corrsponding address:" << sock.getpeername();

        if (sock.status() == true)
        {
            T* t = new T(server_, getRandomLoop(), sock);
        }
    }

private:
    HttpServer *server_;
};

};

#endif