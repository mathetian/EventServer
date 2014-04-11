#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include "Socket.h"

class EventLoop;

class SocketHandler
{
protected:
    EventLoop*  m_loop;
    Socket      m_sock;
    int         m_status;
    int         m_delflag;

public:
    SocketHandler(EventLoop* loop) : m_loop(loop)
    {
        m_status  = 0;
        m_delflag = 0;
    }

    virtual ~SocketHandler()
    { }

public:
    virtual void onReceiveMsg()  = 0;
    virtual void onSendMsg()     = 0;
    virtual void onCloseSocket(int st) = 0;

    void registerRead();
    void registerWrite();
    void unRegisterRead();
    void unRegisterWrite();

    Socket getSocket()
    {
        return m_sock;
    }

    EventLoop *getLoop()
    {
        return m_loop;
    }

    EventLoop *getLoop2();

    int getStatus()
    {
        return m_status;
    }

    int setdelflag()
    {
        m_delflag = 1;
    }

    int getdelflag()
    {
        return m_delflag;
    }

    void updateStatus(int val)
    {
        if(m_delflag == 1) return;

        assert((m_status & val) == 0);
        m_status |= val;
    }

    void removeStatus(int val)
    {
        if(m_delflag == 1) return;

        assert((m_status & val) != 0);
        m_status ^= val;
    }

protected:
    void attach();
    void detach();

    friend class EventLoop;
};

#endif