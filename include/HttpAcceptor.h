// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _HTTP_ACC_H
#define _HTTP_ACC_H

#include "Acceptor.h"

namespace sealedserver
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
    HttpAcceptor(EventLoop* loop, int localport) : Handler(loop)
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
        Socket sock = wrap();

        if (sock.status() == true)
        {
            T* t = new T(m_server, getRLoop(), sock);
        }
    }

    void onSendEvent() { }

    void onCloseEvent(ClsMtd st)
    {
        DEBUG << "close listen socket fd: " << m_sock.fd() << " " << st;

        detach();
        m_sock.close();
    }

private:
	HttpServer *m_server;

public:
    Socket wrap()
    {
        NetAddress a;
        Socket sock = m_sock.accept(&a);

        DEBUG << "New Connection, through socket(local): " << sock.fd();
        DEBUG << "Corrsponding address:" << sock.getpeername();

        return sock;
    }

    EventLoop* getRLoop()
    {
        return getRandomLoop();
    }
};

};

#endif