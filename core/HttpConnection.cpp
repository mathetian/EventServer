// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.


#include "HttpServer.h"
#include "HttpConnection.h"

namespace sealedserver
{

HttpConnection::HttpConnection(HttpServer *server, EventLoop *loop, Socket sock) : \
	MSGHandler(loop, sock), server_(server), errcode_(0)
{
}

HttpConnection::~HttpConnection()
{
}

void HttpConnection::receivedMsg(STATUS status, Buffer &receivedBuff)
{
	parse(receivedBuff);
	
	if(errcode_ != 0) onCloseEvent(CLSMAN);
	else server_ -> process(this);

	unRegisterRead();
}

void HttpConnection::sentMsg(STATUS status, int len, int targetLen)
{
	close();
}

void HttpConnection::parse(Buffer &receivedBuff)
{
	/// TBD
	check();
}

void HttpConnection::check()
{
	/// TBD
}

map<string, string> HttpConnection::getHeader()
{
	return header_;
}

void HttpConnection::notfound()
{
	/// TBD
}

};