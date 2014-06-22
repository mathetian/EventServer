// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "EventLoop.h"
#include "MsgHandler.h"

namespace sealedserver
{

MSGHandler::MSGHandler(EventLoop* loop, Socket sock)
    : Handler(loop), m_global(true), m_first(false)
{
    /// Must be written here
    /// Don't allow after `:`
    m_sock = sock;
    attach();
    registerRead();
    assert(sock.status());
}

MSGHandler::~MSGHandler()
{
    /// Nothing need to be done
    /// Socket has been closed in other functions
}

int MSGHandler::write(const Buffer& buf)
{
    if(m_global == false) return 0;

    m_Bufs.push_back(buf);
    registerWrite();

    return 0;
}

int MSGHandler::close()
{
    onCloseEvent(CLSMAN);
    m_global = false;

    return 0;
}

void MSGHandler::onReceiveEvent()
{
    bool first = true, flag = true;

    while(flag && m_global)
    {
        Buffer buf(MSGLEN);
        int len = m_sock.read(buf.data(), MSGLEN);

        /// if len == 0, means EOF
        /// else len < 0 & errno == EAGAIN, means EWouldBlock(read buffer is empty).
        /// else len < 0, means error(closed the socket)
        /// else len >= MSGLEN, means exceeded the limitation(should be processed manually)
        /// else means normal

        flag = false;

        if(len == 0)
            onCloseEvent(CLSEOF);
        else if(len < 0 && errno == EAGAIN)
        {
            /// omit, shouldn't happen in the first `read`

            if(first == true)
                assert(0);
        }
        else if(len < 0)
            onCloseEvent(CLSERR);
        else if(len < MSGLEN)
        {
            buf.set_length(len);
            receivedMsg(SUCC, buf);
        }
        else
        {
            /// must be proceed manually
            buf.set_length(len);
            receivedMsg(SUCC, buf);
            flag = true;
            first = false;
        }
    }
}

void MSGHandler::onSendEvent()
{
    /// Write utils the buffer empty
    /// Or out buffer is full
    unRegisterWrite();

    if(m_first == true)
    {
        connected();
        m_first = false;
    }


    bool flag = true;

    while(m_Bufs.size() > 0 && flag == true && m_global)
    {
        Buffer buf = m_Bufs.front();
        m_Bufs.pop_front();

        const void *data = buf;
        uint32_t  length = buf.length();

        int len = m_sock.write(data, length);

        /// if len == 0, EOF
        /// else if len < 0 && errno == EAGAIN, out buffer is full
        /// else if len < 0, Error happens. Close the socket
        /// else normal

        flag = false;

        if(len == 0)
            onCloseEvent(CLSEOF);
        else if(len < 0 && errno == EAGAIN)
        {
            m_Bufs.push_back(buf);
            registerWrite();
        }
        else if(len < 0)
            onCloseEvent(CLSERR);
        else
        {
            assert(len == length);
            sentMsg(SUCC, len, length);
            flag = false;
        }
    }
}

void MSGHandler::onCloseEvent(ClsMtd st)
{
    /// the flow of close
    /// 1. detach from loop
    /// 2. close the socket(shutdown?)
    /// 3. invoke the close event of customed
    /// 4. remove this object(add to the wait list of removed items)

    detach();
    m_sock.close();
    closed(st);
    m_loop -> addClosed(this);
}

};