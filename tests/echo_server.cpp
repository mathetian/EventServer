#include <iostream>
using namespace std;

#define PORT 10000

#include "EventLoop.h"
#include "MSGHandler.h"
#include "Socket.h"
#include "TCPAcceptor.h"
using namespace sealedServer;

EventLoop loop("EPoll");

class EchoServer : public MSGHandler
{
public:
    EchoServer(EventLoop& loop, Socket sock) : MSGHandler(loop, sock)
    {
        registerRead();
    }

    ~EchoServer()
    { }

private:
    virtual void receivedMsg(STATUS status, Buffer &buf)
    {
        if(status == SUCC)
        {
            INFO << "Received(from Socket: " << getSocket() << "): " << buf.data();
            write(buf);
        }
    }

    virtual void sendedMsg(STATUS status, int len, int targetLen)
    {
        if(status == SUCC)
        {
            INFO << "sendedMsg: " << len << " " << targetLen << " for socket: " << getSocket();
        }
        else WARN << "Some Error" ;
    }

    virtual void TimerEvent()
    {
        INFO << "Timer Event Start: " << getSocket();
        sleep(2);
        INFO << "Timer Event End: " << getSocket();
    }

    virtual void closedSocket()
    {
        INFO << "Socket need to be closed: " << getSocket();
    }
};

int main()
{
    TCPAcceptor<EchoServer> acceptor(loop, PORT);

    loop.runforever();

    return 0;
}