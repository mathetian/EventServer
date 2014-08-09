// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Socket.h"
#include "EventPool.h"
#include "EventLoop.h"
#include "MSGHandler.h"
using namespace eventserver;

#define BASE_PORT 10000
#define PORT_NUM  1

#define CLIENT_NUM 100000

EventPool pool(1);

/**
** A handler for a single connection to the server
**
** We subclass MSGHandler and override a few of its virtual methods:
**
**   - connected()    is invoked when a connection we try either succeeds or fails.
**   - received()  is invoked when a message is received
**   - sent()      is invoked when a msg has been sent
**   - closed()       is invoked when the socket has been closed
**/
class EchoClient : public MSGHandler
{
public:
    EchoClient(EventLoop *loop, Socket sock) : MSGHandler(loop, sock)
    {
        DEBUG << m_sock.getpeername() << " " << sock.fd();
        m_first = true;
        registerWrite();
    }

    virtual ~EchoClient()
    { }

protected:
    /// Invoked when a connection we try either succeeds or fails.
    virtual void connected()
    {
        DEBUG << "Connected Successful" << m_sock;

        char * buff = new char[20];
        memset(buff, 0, 20);
        sprintf(buff, "wait for me, I am %d", m_sock.fd());

        send(Buffer(buff, true));
    }

    /// Invoked when a message is received
    virtual void received(STATUS status, Buffer &receivedBuff)
    {
        if(status == SUCC)
        {
            DEBUG << "Received(from " <<  m_sock.fd() <<  " ):" << (string)receivedBuff;
        }
        else assert(0);
    }

    /// Invoked when a msg has been sent
    virtual void sent(STATUS status, int len, int targetLen)
    {
        if(status == SUCC)
        {
            DEBUG << "SendedMsg(to " <<  m_sock.fd() <<  "):" << len << " " << targetLen;
        }
        else assert(0);
    }

    /// Invoke when the socket has been closed
    virtual void closed(ClsMtd st)
    {
        DEBUG << "onCloseSocket(for " <<  m_sock.fd() <<  "):" << st;
        if(errno != 0) DEBUG << strerror(errno);

        errno = 0;
    }
};

/// Simulator of clients
/// In it, we will try to create `CLIENT_NUM`s clients (EchoClient)
/// Then, try to create `EchoClient` to bind the socket with this client

class ClientSimulator
{
public:
    ClientSimulator(string ip, int port)
    {
        createClients(ip, port);
    }

    ClientSimulator(int port)
    {
        createClients("127.0.0.1", port);
    }

private:
    void createClients(string ip, int port)
    {
        for(int i = 0; i < CLIENT_NUM; i++)
        {
            NetAddress svrAddr(ip, port + (i%PORT_NUM));

            Socket sock(AF_INET, SOCK_STREAM);

            if(sock.connect(&svrAddr) == false)
            {
                INFO << sock.status() ;
                assert(0);
            }
            int id;
            EchoClient *client = new EchoClient(pool.getRandomLoop(id), sock);

            if(i%10000==0)
            {
                printf("press Enter to continue: ");
                getchar();
            }

            usleep(5 * 1000);
        }
    }
};

/// Signal Stop the server
void signalStop(int)
{
    INFO << "Stop running...by manually";
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

    pool.subrun();

    ClientSimulator simulator(BASE_PORT);

    pool.subjoin();

    INFO << "End of Main";

    return 0;
}