#include <iostream>
using namespace std;

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "../include/EventPool.h"
#include "../include/EventLoop.h"
#include "../include/MsgHandler.h"
#include "../include/Socket.h"

#define BASE_PORT 9000
#define PORT_NUM  10

#define CLIENT_NUM 10000

EventPool pool(1);

class EchoClient : public MSGHandler
{
public:
    EchoClient(EventLoop *loop, Socket sock) : MSGHandler(loop, sock)
    {
        DEBUG << m_sock.getsockname() << " " << sock.get_fd();
    }

    ~EchoClient()
    { }

protected:
    virtual void receivedMsg(STATUS status, Buffer &buf)
    {
        if(status == SUCC)
        {
            INFO << "ReceivedMsg: " << (string)buf << " through fd " << m_sock.get_fd();
        }
    }

    virtual void sendedMsg(STATUS status, int len, int targetLen)
    {
        if(status == SUCC)
        {
            INFO << "SendedMsg: " << len << " " << targetLen << " through fd " << m_sock.get_fd();
        }
    }

    virtual void closedSocket()
    { }

    virtual void onConnected(STATUS status)
    {
        INFO << "Connected Successful";
        write("wait for me");
    }
};

class ClientSimulator
{
public:
    ClientSimulator(string ip, int port)
    {
        Initialize(ip, port);
    }

    ClientSimulator(int port)
    {
        Initialize(port);
    }

    void Initialize(int port)
    {
        NetAddress svrAddr(port);
        createClients(&svrAddr, CLIENT_NUM);
    }

    void Initialize(string ip, int port)
    {
        NetAddress svrAddr(ip, port);
        createClients(&svrAddr, CLIENT_NUM);
    }

private:
    void createClients(Address *psvrAddr, int size)
    {
        for(int i = 0; i < size; i++)
        {
            NetAddress svrAddr(BASE_PORT+(i%PORT_NUM));
            Socket sock(AF_INET, SOCK_STREAM);
            sock.cliConnect(&svrAddr);
            assert(sock.get_fd() >= 0);
            EchoClient *client = new EchoClient(EventPool::getRandomLoop(), sock);
            if(i%10000==0) 
            {   
                printf("press Enter to continue: ");
                getchar();
            }
            usleep(1 * 1000);
        }
    }
};

void signalStop(int)
{
    INFO << "Stop running...by manually";
  //  loop.stop();
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
    ClientSimulator simulator(BASE_PORT);
    pool.runforever();

    return 0;
}