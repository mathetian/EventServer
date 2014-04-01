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
        DEBUG << "MSGHandler Initialiaztion";
        DEBUG << "Peer Information: " << getSocket().getpeername();
        assert(getSocket().stat());

        attach();
    }

    virtual ~MSGHandler()
    {  }

protected:
    int write(const Buffer& buf)
    {
        {
            ScopeMutex scope(&m_mutex);
            m_Bufs.push_back(buf);
        }
        registerWrite();
        // Callback<void> call(this, &MSGHandler::onSendMsg);
        // m_loop->m_pool.insert(call);
    }

    typedef enum  { EXCEED, SOCKERR, BLOCKED, SUCC} STATUS;

private:
    virtual void onReceiveMsg()
    {
        assert(getSocket().stat() == true);

        Buffer buf(MSGLEN);
        int len = getSocket().read(buf.data(), MSGLEN);

        INFO << "Received from :" << getSocket();

        if(len == 0)
        {
            WARN << "Need close the socket: " << getSocket();
            onCloseSocket();
            return;
        }

        if(len < 0 && getSocket().stat() == false)
        {
            WARN << "Socket error: " << getSocket();
            WARN << "Socket will be closed: " << getSocket();
            onCloseSocket();
            return;
        }

        if(len == MSGLEN)
        {
            WARN << "Exceed the buf length";
            receivedMsg(EXCEED, buf);
        }
        else if(len < 0)
        {
            WARN << "EWOULDBLOCK";
            receivedMsg(BLOCKED, buf);
        }
        else
        {
            buf.set_length(len);
            receivedMsg(SUCC, buf);
        }
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
        if(len < 0 && errno == EAGAIN)
        {
            ScopeMutex scope(&m_mutex);
            m_Bufs.push_back(buf);
            registerWrite();
        }
        else if(len < 0)
        {
            WARN << "Write Error, so close socket" ;
            onCloseSocket();
        }
        else
        {
            assert(len == length);
            sendedMsg(SUCC, len, length);
        }
    }

    virtual void onCloseSocket()
    {
        detach();
        closedSocket();
        getSocket().close();
        INFO << "Socket has been closed & obejct will be destructed";

        delete this;
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