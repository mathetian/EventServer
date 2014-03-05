#include "../include/EventLoop.h"
#include "../include/MSGHandler.h"
#include "../include/Socket.h"
#include "../include/TCPAcceptor.h"

#include <iostream>
using namespace std;

#define PORT 10000
#define MSGLEN 512

EventLoop loop;

class EchoServer : public MSGHandler
{
 public:
    EchoServer(EventLoop& loop, Socket sock) : MSGHandler(loop, sock)
    {
       waitWrite(true);
    }

    ~EchoServer()
    { }

  protected:
  	void onReceiveMsg()
  	{
      char buf[MSGLEN];
      memset(buf, 0, MSGLEN);
      int len = read(buf, MSGLEN);
  	  if(len == MSGLEN)
      {
        WARN << "Exceed the buf length";
      }
      else if(len < 0)
      {
        if(getSocket().stat() == false)
        {
          WARN << "Socket error";
          deleteMe();
        }
        else
        {
          //Errno, Need again. No further process
          WARN << "Need Receive Again";
        }
      }
      else if(len == 0)
      {
        WARN << "Socket has been closed";
        deleteMe();
      }
      else
      {
        INFO << "Received from :" << getSocket().getpeername();
        INFO << buf;
      }
    }

  	void onSendMsg()
  	{
      Slice slice("hello, body");
      write(slice);
      DEBUG << "Client onSendMsg:" << getSocket();
      waitRead(true);
  	}

  	void onCloseSocket()
  	{
      /**Todo List**/
  	}

    void onTimer() 
    {
      
    }
};

int main()
{
	TCPAcceptor<EchoServer> acceptor(loop, PORT);

  loop.runforever();
  
	return 0;
}