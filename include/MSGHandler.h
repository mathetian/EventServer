#ifndef _MSG_HANDLER_H
#define _MSG_HANDLER_H

#include "../utils/Slice.h"
#include "../utils/Buffer.h"
#include "../utils/Thread.h"
using namespace utils;

#include <list>
using namespace std;

#include "SocketHandler.h"
#include "EventLoop.h"

#define MSGLEN 512

class MSGHandler : public SocketHandler
{
public:
    MSGHandler(EventLoop& loop, Socket sock) : SocketHandler(loop)
    {
        setSock(sock);
        DEBUG << "MSGHandler Socket STATUS: " << m_sock.stat();
        assert(m_sock.stat());
        DEBUG << "MSGHandler Initialiaztion Successfully";
        DEBUG << "Peer Information: " << m_sock.getpeername();
        attach();
    }

    virtual ~MSGHandler()
    {  }

protected:
    int write(const Buffer& buf)
    {
        ScopeMutex scope(&m_mutex);
        m_Bufs.push_back(buf);
        Callback<void> call(this, &MSGHandler::onSendMsg);
        m_loop->m_pool.insert(call);
    }

    typedef enum  { EXCEED, SOCKERR, BLOCK, SUCC} STATUS;

private:
    virtual void onReceiveMsg()
    {
        Buffer buf(MSGLEN);
        int len = getSocket().read(buf.data(), MSGLEN);

        INFO << "Received from :" << getSocket().getpeername();

        if(len == MSGLEN)
        {
            WARN << "Exceed the buf length";
            receivedMsg(EXCEED, buf);
        }
        else if(len < 0)
        {
            if(getSocket().stat() == false)
            {
                WARN << "Socket error";
                receivedMsg(SOCKERR, buf);
            }
            else
            {
                WARN << "EWOULDBLOCK";
                receivedMsg(BLOCK, buf);
            }
        }
        else if(len == 0) onCloseSocket();
        else
        {
            buf.set_length(len);
            receivedMsg(SUCC, buf);
        }
    }

    virtual void onCloseSocket()
    {
        WARN << "Socket has been closed";
        closedSocket();
        detach();
    }

    virtual void onSendMsg()
    {
        Buffer buf;
        {
            ScopeMutex scope(&m_mutex);
            assert(m_Bufs.size() != 0);
            buf = m_Bufs.front();
            m_Bufs.pop_front();
        }
        const void *data = buf;
        uint32_t length  = buf.length();

        int len = getSocket().write(data, length);
        sendedMsg(SUCC, len, length);
    }

public:
    virtual void receivedMsg(STATUS status, Buffer &buf) { }
    virtual void sendedMsg(STATUS status, int len, int targetLen) { }
    virtual void TimerEvent()                { }
    virtual void closedSocket()              { }

private:
    list<Buffer>   m_Bufs;
    Mutex          m_mutex;
};

#endif