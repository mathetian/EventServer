#include "../include/EventLoop.h"
#include "../include/MSGHandler.h"
#include "../include/Socket.h"
#include "../include/TCPAcceptor.h"

#include <iostream>
using namespace std;

#define PORT 10000
#define CLIENT_NUM 100
#define MSGLEN 256

EventLoop loop;

class EchoClient : public MSGHandler
{
 public:
    EchoClient(EventLoop& loop, Socket sock) : MSGHandler(loop, sock)
    {
		cout << "Accepted connected from " << getSocket().getpeername() <<
	    	" on " << getSocket().getsockname() << endl;
	    waitRead(true);
    }

    ~EchoClient()
    { }

  protected:
  	void onReceiveMsg()
  	{
  	  char buf[MSGLEN];
      memset(buf, 0, MSGLEN);
      int len = read(buf, MSGLEN);
  	  if(len == MSGLEN)
      {
        cout<<"Exceed the buf length"<<endl;
      }
      else if(len < 0)
      {
        if(getSocket().stat() == false)
        {
          cout<<"Socket error"<<endl;
          deleteMe();
        }
        else
        {
          //Errno, Need again. No further process
        }
      }
      else
      {
        cout<<"Received from:"<<getSocket().getsockname().as_string()<<endl;
        cout<<buf<<endl;
      }
      waitRead(false);waitWrite(true);
  	}

  	void onSendMsg()
  	{
  	  Slice slice("hello, server");
      write(slice);
      deleteMe();
  	}

  	void onCloseSocket()
  	{
  		//Todo List
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
      DEBUG << sock.stat().to_string();
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