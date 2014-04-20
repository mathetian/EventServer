#ifndef _MSG_HANDLER_H
#define _MSG_HANDLER_H

#include <list>
using namespace std;

#include "../utils/Buffer.h"
#include "../utils/Log.h"
using namespace utils;

#include "SocketHandler.h"

#define MSGLEN 1024

class MSGHandler : public SocketHandler
{
public:
    MSGHandler(EventLoop* loop, Socket sock, int first=0);

public:
    int write(const Buffer& buf)
    {
        m_Bufs.push_back(buf);
        //registerWrite();
        onSendMsg();
    }

    int close()
    {
        onCloseSocket(CLSMAN);
    }

    typedef enum  { EXCEED, SOCKERR, BLOCKED, SUCC} STATUS;

private:
    void onReceiveMsg()
    {
        Buffer buf(MSGLEN);
        int len = m_sock.read(buf.data(), MSGLEN);

        if(len == 0)  onCloseSocket(CLSEOF);
        else if(len < 0 && errno != EAGAIN) onCloseSocket(CLSERR);
        else if(len == MSGLEN) receivedMsg(EXCEED, buf);
        else if(len < 0) receivedMsg(BLOCKED, buf);
        else
        {
            buf.set_length(len);
            receivedMsg(SUCC, buf);
        }
    }

    void onSendMsg()
    {
        if(first == 0)
        {
            int       error = 0;
            socklen_t errlen = sizeof(error);
            assert(getsockopt(m_sock.get_fd(), SOL_SOCKET, SO_ERROR, (void *)&error, &errlen) == 0);
            if(error != 0)
            {
                DEBUG << strerror(error);
                onCloseSocket(CLSWRR);
            }
            else
            {
                first = 1;
                onConnected(SUCC);
            }
            return;
        }

        while(m_Bufs.size() > 0)
        {
            Buffer buf = m_Bufs.front();
            m_Bufs.pop_front();

            const void *data = buf;
            uint32_t length  = buf.length();

            int len = m_sock.write(data, length);
            if(len < 0 && errno == EAGAIN)
            {
                m_Bufs.push_back(buf);
                registerWrite();
                break;
            }
            else if(len < 0)
            {
                onCloseSocket(CLSWRR);
                break;
            }
            else sendedMsg(SUCC, len, length);
        }
    }

    void onProceed()
    {
        attach(); registerRead();
        if(first==0) registerWrite();
    }

public:
    virtual void onCloseSocket(int st);

public:
    virtual void receivedMsg(STATUS status, Buffer &buf) { }
    virtual void sendedMsg(STATUS status, int len, int targetLen) { }
    virtual void closedSocket()              { }
    virtual void onConnected(STATUS status)  { }

private:
    list<Buffer>   m_Bufs;
    int            first;
};

#endif