// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Timer.h"
#include "Thread.h"
using namespace utils;

#include "Socket.h"
#include "EventLoop.h"
#include "EventPool.h"
#include "MsgHandler.h"
using namespace eventserver;

/// count, the total has been received
/// writes, the number of item need to be written
/// fired, the number of items has been written
///
/// We should notice that `fired` + `writes` = `A fixed value`
Atomic count, writes, fired;

/// num_pipes,  number of socket-pairs
/// num_active, number of active at the same time
/// num_writes, number of times of writting(need to send `num_writes` times)
int num_pipes, num_active, num_writes;

class EchoServer;

/// socket-pairs
vector<Socket> pipes;

/// each socket will bind with a server instance
/// we put it in the vector
vector<EchoServer*> servers;

EventPool pool(4);

Timer start, end, prev;

class EchoServer : public MSGHandler
{
public:
    EchoServer(EventLoop *loop, Socket sock, int idx) : MSGHandler(loop, sock), idx(idx)
    {
        DEBUG << m_sock.getpeername() << " " << sock.fd();
    }

    void clear()
    {
        m_status = 0;
        m_delflag = 0;
    }

private:
    virtual void received(STATUS status, Buffer &receivedBuff)
    {
        if(status == SUCC)
        {
            DEBUG << "Received(from " <<  m_sock.fd() <<  " ):" << (string)receivedBuff;
            count += receivedBuff.length();

            if (writes > 0)
            {
                int widx = idx + 1;
                if (widx >= num_pipes) widx -= num_pipes;

                if(::write(pipes[2 * widx + 1].fd(), "m", 1) != 1)
                {
                    INFO << strerror(errno) ;
                    assert(0);
                }

                writes--;
                fired++;
            }

            if(count % num_pipes == 0)
            {
                end = Timer::now();
                fprintf(stdout, "%8ld, id %d\n", end.to_usecs() - start.to_usecs(), (int)(Thread::getIDType()%100));
                // fprintf(stdout, "%8ld, %8ld\n", end.to_usecs(), start.to_usecs());
                fprintf(stdout, "writes=%d, fired=%d, recv=%d\n", (int)writes, (int)fired, (int)count);
                start = end;
            }
        }
        else assert(0);
    }

    // Invoke when the socket has been closed
    virtual void closed(ClsMtd st)
    {
        DEBUG << "onCloseSocket(for " <<  m_sock.fd() <<  " ):" << st;
        if(errno != 0) DEBUG << strerror(errno);

        errno = 0;
    }

private:
    int idx;
};

void run_once()
{
    int space = num_pipes / num_active;
    space *= 2;

    for (int i = 0; i < num_active; i++)
        ::write(pipes[i*space+1].fd(),"m",1);

    if(errno != 0)
        printf("error: %s\n",strerror(errno));
    errno = 0;

    fired  = num_active;
    count  = 0;
    writes = num_writes;

    start = Timer::now();

    WARN << "Begin Run" ;

    pool.run();

    WARN << "End Run" ;

    end = Timer::now();

    fprintf(stdout, "%8ld\n", end.to_usecs() - start.to_usecs());
}

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

int main(int argc, char* argv[])
{
    Log::setLevel(Log::info);

    num_pipes  = 100000;
    num_active = 100;
    num_writes = num_pipes*25;

    setlimit(num_pipes);

    int c;
    while ((c = getopt(argc, argv, "n:a:w:")) != -1)
    {
        switch (c)
        {
        case 'n':
            num_pipes = atoi(optarg);
            break;
        case 'a':
            num_active = atoi(optarg);
            break;
        case 'w':
            num_writes = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Illegal argument `%c`\n", c);
            return 1;
        }
    }
    num_writes*=25;

    pipes = vector<Socket>(num_pipes*2,-1);

    for (int i = 0; i < num_pipes; i++)
    {
        pair<Socket, Socket> ss = Socket::pipe();
        pipes[i*2]   = ss.first;
        pipes[i*2+1] = ss.second;
    }

    for (int i = 0; i < num_pipes; i++)
        servers.push_back(new EchoServer(pool.getRandomLoop(), pipes[i*2],i));

    run_once();
    fprintf(stdout, "writes=%d, fired=%d, recv=%d\n", (int)writes, (int)fired, (int)count);
    pool.stop();
}
