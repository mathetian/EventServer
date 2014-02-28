#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include "Socket.h"

class EventLoop;

class SocketHandler 
{
private:
	EventLoop*  m_loop;
    Socket      m_sock;
    bool        m_waitRead;
    bool        m_waitWrite;

public:
    SocketHandler(Socket sock = Socket()) : m_loop(0), m_sock(sock), m_waitRead(false), m_waitWrite(false) {}

    SocketHandler(EventLoop& loop, Socket sock = Socket()) :
    	 m_loop(&loop), m_sock(sock), m_waitRead(false), m_waitWrite(false)
    {
        attach();
    }

    virtual ~SocketHandler() 
    {
        detach();
    }

public:
    //Todo List
    virtual void readAvail()  { }

    virtual void writeAvail() { }

    void waitRead(bool);

    void waitWrite(bool);

    void   setSock(Socket sock)
    {
        m_sock = sock;
    }

    Socket getSock() 
    { 
    	return m_sock; 
    }

    EventLoop *getLoop()
    {
        return m_loop;
    }
    
private:
    void attach();
    void detach();
    
    friend class EventLoop;
};

#endif