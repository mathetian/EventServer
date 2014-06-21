#include <iostream>
using namespace std;

#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "EventPool.h"
#include "EventLoop.h"
#include "MsgHandler.h"
#include "Socket.h"
#include "Acceptor.h"

#define BASE_PORT 10000
#define PORT_NUM  10

EventPool pool(4);

class EchoServer : public MSGHandler
{
public:
    EchoServer(EventLoop *loop, Socket sock) : MSGHandler(loop, sock,1)
    { }

    ~EchoServer()
    { }

private:
    virtual void receivedMsg(STATUS status, Buffer &buf)
    {
        if(status == SUCC)
        {
            INFO << "Received: " << (string)buf << " through fd " << m_sock.fd();
            write(buf);
        }
    }

    virtual void sendedMsg(STATUS status, int len, int targetLen)
    {
        if(status == SUCC)
        {
            INFO << "SendedMsg: " << len << " " << targetLen << " through fd " << m_sock.fd();
        }
        else { }
    }

    virtual void closedSocket()
    { }
};

void signalStop(int)
{
    INFO << "Stop running...by manually";
    pool.closeAllLoop();
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

int main()
{
    ::signal(SIGINT, signalStop);
    setlimit(100000);
    errno = 0;
    vector<TCPAcceptor<EchoServer>*> acceptors(PORT_NUM, NULL);

    for(int i = 0; i < PORT_NUM; i++)
        acceptors[i] = new TCPAcceptor<EchoServer>(pool.getRandomLoop(), BASE_PORT+i);

    pool.runforever();
    INFO << "End of Main" ;

    for(int i = 0; i < PORT_NUM; i++) delete acceptors[i];

    return 0;
}