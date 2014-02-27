#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include "Socket.h"

class EventLoop;

class SocketHandler 
{
private:
	EventLoop*  m_loop;
    Socket      m_sck;
    bool cur_want_read;
    bool cur_want_write;
    bool owned;

public:
    SocketHandler(Socket sck = Socket()) : m_loop(0), m_sck(sck), cur_want_read(false), cur_want_write(false), owned(true) {}

    SocketHandler(EventLoop& loop, Socket sck = Socket(), bool owned = false) :
    	 m_loop(&loop), m_sck(sck), cur_want_read(false), cur_want_write(false), owned(true)
    {
        attach();
    }

    virtual ~SocketHandler() 
    {
        detach();
    }

public:

    void setSock(Socket sck) 
    {
        detach();
        m_sck = sck;
        attach();
    }

    void readAvail() {}

    void writeAvail() {}

    void want_read(bool);

    void want_write(bool);

    Socket getSock() 
    { 
    	return m_sck; 
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