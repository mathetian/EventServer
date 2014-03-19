#include "../include/EventLoop.h"
#include "../include/MSGHandler.h"
#include "../include/Socket.h"
#include "../include/TCPAcceptor.h"

#include <iostream>
using namespace std;

#define PORT 10000
#define CLIENT_NUM 5

EventLoop loop;

class EchoClient : public MSGHandler
{
public:
    EchoClient(EventLoop& loop, Socket sock) : MSGHandler(loop, sock)
    {
        waitTimer(1);
        write("wait for me");
    }

    ~EchoClient()
    { }

protected:
    virtual void receivedMsg(STATUS status, Buffer &buf)
    {
        if(status == SUCC)
            INFO << "Received(from Socket: " << getSocket() << "): " << buf.data();
    }

    virtual void sendedMsg(STATUS status, int len, int targetLen)
    {
        if(status == SUCC)
            INFO << "SendedMsg: " << len << " " << targetLen << " for socket: " << getSocket();
        else
            WARN << "Some Error" ;
    }

    virtual void TimerEvent()
    {
        INFO << "Timer Event Start: " << getSocket();
        sleep(2);
        INFO << "Timer Event End: " << getSocket();
    }

    virtual void closedSocket()
    {
        INFO << "Socket has been closed, will remove it from memory soon.";
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
            Socket sock(AF_INET, SOCK_STREAM, psvrAddr);
            DEBUG << "CreateClients: " << sock << " status: " << sock.stat();
            EchoClient *client = new EchoClient(loop, sock);
        }
    }
};

int main()
{
    ClientSimulator simulator(PORT);
    loop.runforever();

    return 0;
}