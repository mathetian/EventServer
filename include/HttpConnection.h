// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _HTTP_CONN_H
#define _HTTP_CONN_H

#include "Socket.h"
#include "Acceptor.h"
#include "EventPool.h"
#include "EventLoop.h"
#include "MsgHandler.h"
using namespace sealedserver;

namespace sealedserver
{

class HttpServer;

class HttpConnection : public MSGHandler
{
    typedef map<string, string> Header;
public:
	/// Constructor
	HttpConnection(HttpServer *server, EventLoop *loop, Socket sock);
	
	/// Destructor
	virtual ~HttpConnection();

public:
	/// Invoked when a message is received.
    virtual void receivedMsg(STATUS status, Buffer &receivedBuff);

    /// Invoked when a msg has been sent
    virtual void sentMsg(STATUS status, int len, int targetLen);

public:
    map<string, string> getHeader();
    void   notfound();
    
private:
    void parse(Buffer &receivedBuff);
    void check();

private:
    Header       header_;
    string       body_;
    HttpServer  *server_;
    int          errcode_;
};

};

#endif