#include "../include/EventLoop.h"
#include "../include/MSGHandler.h"
#include "../include/Socket.h"
#include "../include/TCPAcceptor.h"

#include <iostream>
using namespace std;

#define PORT 10000
#define CLIENT_NUM 5
#define MSGLEN 256

EventLoop loop;

class EchoClient : public MSGHandler
{
 public:
    EchoClient(EventLoop& loop, Socket sock) : MSGHandler(loop, sock)
    {
      waitRead(true);
      waitTimer(1);
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
          ERROR << "Socket error";
          deleteMe();
        }
        else
        {
          //Errno, Need again. No further process
        }
      }
      else if(len == 0)
      {
        WARN << "Socket has been closed";
        deleteMe();
      }
      else
      {
        INFO << "Received from: " << getSocket().getsockname();
        INFO << buf;
        waitWrite(true);
        waitRead(true);
      }
  	}

  	void onSendMsg()
  	{
  	  Slice slice("hello, server");
      write(slice);
      DEBUG << "onSendMsg: " << getSocket();
      //waitRead(true);
  	}

  	void onCloseSocket()
  	{
  		//Todo List
  	}

    void onTimer()
    {
      INFO << "Timer Event Start: " << getSocket();
      sleep(2);
      INFO << "Timer Event End: " << getSocket();
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
      DEBUG << sock.stat().as_string();
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