// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Socket.h"
#include "Acceptor.h"
#include "EventPool.h"
#include "EventLoop.h"
#include "MsgHandler.h"
using namespace eventserver;

#define BASE_PORT 10000
#define PORT_NUM  1

EventPool pool(4);

/**
** A handler for a single connection to a client.
**
** In this application, EchoServers are created
** by the TCPAcceptor<EchoServer> handler
** see the main() function below.
**
** We subclass MSGHandler and override a few of its virtual methods:
**
** - received()  is invoked when a message is received
** - sent()      is invoked when a msg has been sent
** - closed()       is invoked when the socket has been closed
**/
class EchoServer : public MSGHandler
{
public:
    EchoServer(EventLoop *loop, Socket sock) : MSGHandler(loop, sock)
    { }

    virtual ~EchoServer()
    { }

private:
    /// Invoked when a message is received.
    virtual void received(STATUS status, Buffer &receivedBuff)
    {
        if(status == MSGHandler::SUCC)
        {
            DEBUG << "Received(from " <<  m_sock.fd() <<  "):" << (string)receivedBuff;
            send(receivedBuff);
        }
        else  assert(0);
    }

    // Invoked when a msg has been sent
    virtual void sent(STATUS status, int len, int targetLen)
    {
        if(status == MSGHandler::SUCC)
        {
            DEBUG << "SendedMsg(to " <<  m_sock.fd() <<  "):" << len << " " << targetLen;
        }
        else assert(0);
    }

    // Invoked when the socket has been closed
    virtual void closed(ClsMtd st)
    {
        DEBUG << "onCloseSocket(for " <<  m_sock.fd() <<  "):" << st;
        if(errno != 0) DEBUG << strerror(errno);

        errno = 0;
    }
};

/// Signal Stop the server
void signalStop(int)
{
    INFO << "Stop running...by manual";
    pool.stop();
}

/// Change the configure
int setlimit(int num_pipes)
{
    struct rlimit rl;
    rl.rlim_cur = rl.rlim_max = num_pipes * 2 + 50;
    if (::setrlimit(RLIMIT_NOFILE, &rl) == -1)
    {
        fprintf(stderr, "setrlimit error: %s", strerror(errno));
        return 1;
    }
}

int main()
{
    Log::setLevel(Log::debug);

    ::signal(SIGINT, signalStop);
    setlimit(100000);
    errno = 0;

    vector<TCPAcceptor<EchoServer>*> acceptors(PORT_NUM, NULL);

    for(int i = 0; i < PORT_NUM; i++)
        acceptors[i] = new TCPAcceptor<EchoServer>(pool.getRandomLoop(), BASE_PORT + i);

    pool.run();

    for(int i = 0; i < PORT_NUM; i++) delete acceptors[i];

    INFO << "End of Main" ;

    return 0;
}