// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "EventLoop.h"
#include "MsgHandler.h"

namespace sealedserver
{

MSGHandler::MSGHandler(EventLoop* loop, Socket sock, int first) : Handler(loop)
{
    m_sock = sock;
    m_loop->insert(this);
}

MSGHandler::~MSGHandler()
{
}

int MSGHandler::write(const Buffer& buf)
{
    m_Bufs.push_back(buf);
    onSendMsg();
}

int MSGHandler::close()
{
    onCloseEvent(CLSMAN);
}

void MSGHandler::onReceiveEvent()
{
    Buffer buf(MSGLEN);
    int len = m_sock.read(buf.data(), MSGLEN);

    if(len == 0)  
        onCloseSocket(CLSEOF);
    else if(len < 0 && errno != EAGAIN) 
        onCloseSocket(CLSERR);
    else if(len == MSGLEN) 
        receivedMsg(EXCEED, buf);
    else if(len < 0) 
        receivedMsg(BLOCKED, buf);
    else
    {
        buf.set_length(len);
        receivedMsg(SUCC, buf);
    }
}

void MSGHandler::onSendEvent()
{
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
            onCloseSocket(CLSERR);
            break;
        }
        else 
            sendedMsg(SUCC, len, length);
    }
}

void MSGHandler::onCloseEvent(ClsMtd st)
{
    if(errno != 0) 
        DEBUG << strerror(errno);
    DEBUG << "onCloseSocket: " << st << " " << m_sock.fd();
    errno = 0;

    detach();
    closedSocket();
    m_sock.close();
    m_loop->addClosed(this);
}

};