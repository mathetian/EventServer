#include "../include/EventLoop.h"
#include "../include/MSGHandler.h"
#include "../include/Socket.h"
#include "../include/TCPAcceptor.h"

#include <iostream>
using namespace std;

#define PORT 10000
#define MSGLEN 256

EventLoop loop;

class EchoServer : public MSGHandler
{
 public:
    EchoServer(EventLoop& loop, Socket sock) : MSGHandler(loop, sock)
    {
		  cout << "Accepted connected from " << getSocket().getpeername() <<
	    	  " on " << getSocket().getsockname() << endl;
        
        /**Flags must be setted here, or add timer here**/
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

      deleteMe();
    }

  	void onSendMsg()
  	{
      Slice slice("hello, body");
      write(slice);
      waitWrite(false);waitRead(true);
  	}

  	void onCloseSocket()
  	{
      /**Todo List**/
  	}
};

int main()
{
	TCPAcceptor<EchoServer> acceptor(loop, PORT);

  loop.runforever();
  
	return 0;
}