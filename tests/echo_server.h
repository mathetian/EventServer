#include "../include/EventLoop.h"
#include "../include/MSGHandler.h"
#include "../include/Socket.h"
#include "../include/TCPAcceptor.h"

#include <iostream>
using namespace std;

#define PORT 10000

EventLoop loop;

class EchoServer : public MSGHandler
{
 public:
    EchoServer(EventLoop& loop, Socket sock) : MSGHandler(loop, sock)
    {
		cout << "Accepted connected from " << getSock().getpeername() <<
	    	" on " << getSock().getsockname() << endl;
    }

    ~EchoServer()
    {
    }

  protected:
  	void OnReceiveMsg()
  	{
  	}

  	void onSendMsg()
  	{
  	}

  	void onCloseSocket()
  	{
  	}
};

int main()
{
	TCPAcceptor<EchoServer> acceptor(loop, PORT);

  loop.runforever();
  
	return 0;
}