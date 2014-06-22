// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _MSG_HANDLER_H
#define _MSG_HANDLER_H

using namespace std;

#include "Log.h"
#include "Buffer.h"
using namespace utils;

#include "Handler.h"

#define MSGLEN 1024

namespace sealedserver
{

/**
** A handler which reads and writes messages
** Handlers should override connected (inherited from NetHandler);
** incoming_message(const Header&, databuf) to read messages; and
** end_messages(unsigned int) to be notified of stream closure.
**/
class MSGHandler : public Handler
{
public:
    /// Constructor
    ///
    /// @param loop, EventLoop it belongs
    /// @param sock, the socket
    MSGHandler(EventLoop* loop, Socket sock);

    /// Destructor
    virtual ~MSGHandler();

public:
    /// writes a message, put it into a queue
    ///
    /// @param buf, the buffer needed to be written
    int write(const Buffer& buf);

    /// close the socket manually
    int close();

    /// status enumeration
    typedef enum  { EXCEED, SOCKERR, BLOCKED, SUCC, CONTINUE, FINAL} STATUS;

private:
    /// Invoked when a written event happens(callback)
    void onReceiveEvent();

    /// Invoked when a read event happens(callback)
    void onSendEvent();

    /// Invoked when a close event happens(callback)
    void onCloseEvent(ClsMtd st);

public:
    /// Invoke when has connected to the server
    /// Should override if and only if client
    virtual void connected()  { }

    /// Invoke when a message has been received
    virtual void receivedMsg(STATUS status, Buffer &buf) { }

    /// Invoke when a message has been sent
    virtual void sentMsg(STATUS status, int len, int targetLen) { }

    /// Invoke when a socket has been closed
    virtual void closed(ClsMtd st)              { }

private:
    /// Buffer list, which wait for write
    list <Buffer>  m_Bufs;

    /// Global flag for closed
    bool           m_global;

protected:
    /// Flag for connected(certainly only for client)
    bool           m_first;
};

};

#endif