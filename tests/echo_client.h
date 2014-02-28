#include "../include/EventLoop.h"
#include "../include/MSGHandler.h"
#include "../include/Socket.h"
#include "../include/TCPAcceptor.h"

#include <iostream>
using namespace std;

#include <assert.h>

#define PORT 10000
#define CLIENT_NUM 100

EventLoop loop;

class EchoClient : public MSGHandler
{
 public:
    EchoClient(EventLoop& loop, Socket sock) : MSGHandler(loop, sock)
    {
		cout << "Accepted connected from " << getSock().getpeername() <<
	    	" on " << getSock().getsockname() << endl;
    }

    ~EchoClient()
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
		createClients(svrAddr, CLIENT_NUM);
	}

	void Initialize(string ip, int port)
	{
		NetAddress svrAddr(ip, port);
		createClients(svrAddr, CLIENT_NUM);
	}

private:
	void createClients(Address &svrAddr, int size)
	{
		for(int i = 0; i < size; i++)
		{
			Socket sock(AF_INET, SOCK_STREAM, svrAddr);
			assert(sock.stat() == true);
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