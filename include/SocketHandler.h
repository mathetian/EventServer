#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include "Socket.h"

#include "../utils/Log.h"
#include "../utils/TimeStamp.h"

class EventLoop;

class SocketHandler
{
protected:
    EventLoop*  m_loop;
    Socket      m_sock;

public:
    SocketHandler(EventLoop& loop) : m_loop(&loop)
    { }

    virtual ~SocketHandler()
    { }

public:
    //Todo List
    virtual void onReceiveMsg()  = 0;
    virtual void onSendMsg()     = 0;
    virtual void onCloseSocket() = 0;
    virtual void TimerEvent()       = 0;

    void waitTimer(int);
    void waitTimer(const TimeStamp &tms);
    void setSock(Socket sock)
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

    void closeSocketForLoop()
    {
        // getSocket().close();
        INFO << "Socket has been closed & obejct has been destructed";
    }

protected:
    void attach();
    void detach();

    friend class EventLoop;
};

#endif