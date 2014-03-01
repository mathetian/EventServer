#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include "Socket.h"

#include "../utils/Log.h"

class EventLoop;

class SocketHandler 
{
protected:
	EventLoop*  m_loop;
    Socket      m_sock;
    bool        m_waitRead;
    bool        m_waitWrite;

public:
    SocketHandler(Socket sock = Socket()) : m_loop(0), m_sock(sock), m_waitRead(false), m_waitWrite(false) { }

    SocketHandler(EventLoop& loop, Socket sock = Socket(), bool r=true, bool w=true) :
    	 m_loop(&loop), m_sock(sock), m_waitRead(r), m_waitWrite(w)
    {
        attach();
        DEBUG << "ATTACH SUCCESSFULLY";
        waitRead(m_waitRead);
        waitWrite(m_waitWrite);
    }

    virtual ~SocketHandler() 
    {
        detach();
    }

public:
    //Todo List
    virtual void onReceiveMsg() = 0;
    virtual void onSendMsg() = 0;
    virtual void onCloseSocket() = 0;
    void waitRead(bool);

    void waitWrite(bool);

    void   setSock(Socket sock)
    {
        m_sock = sock;
    }

    Socket getSocket() 
    { 
    	return m_sock; 
    }

    EventLoop *getLoop()
    {
        return m_loop;
    }
    
protected:
    void attach();
    void detach();
    
    friend class EventLoop;
};

#endif