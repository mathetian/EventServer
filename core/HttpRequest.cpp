// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpClient.h"
#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace sealedserver
{

HttpRequest::HttpRequest(HttpServer *server, EventLoop *loop, Socket sock) : \
    MSGHandler(loop, sock), server_(server), errcode_(0), first_(false), client_(NULL)
{
    /// TBD
    parser_ = new HttpParser(0);
}

HttpRequest::HttpRequest(HttpClient *client, string url, EventLoop *loop, Socket sock) : \
    MSGHandler(loop, sock), server_(NULL), errcode_(0), first_(false), client_(client), url_(url)
{
    /// TBD
    parser_ = new HttpParser(1);
    m_first = true;
    registerWrite();
}

HttpRequest::~HttpRequest()
{
    if(response_)
        delete response_;

    response_ = NULL;

    delete parser_;
    parser_ = NULL;
}

/// Only used by client
void HttpRequest::connected()
{
    /// TBD

    /// Packet the basic information
    /// Into a suitable packet
    /// And send it to the server

    response_ = new HttpResponse(this);

    response_ -> send();
}

void HttpRequest::received(STATUS status, Buffer &receivedBuff)
{
    /// Forbid received twice
    /// We don't use unRegisterRead 
    /// As it will incur the forbidded vist
    if(first_ == true) return;

    first_ = true;

    bool flag = parser_ -> parse(receivedBuff);

    if(flag == false){
        errcode_ = 1;
    }

    if(server_ != NULL)
    {
        if(errcode_ != 0) {
            initResponse(400);
            badRequest();
        } else server_ -> process(this);
    }
    else
    {
        /// TBD
        client_ -> process(this);
    }
}

void HttpRequest::sent(STATUS status, int len, int targetLen)
{
    if(server_ != NULL)
        close();
}

/// Not found & without error callback
void HttpRequest::notFound()
{
    response_ -> addHeader("Content-Type", "text/html");

    response_ -> addBody("<HTML><TITLE>Not Found</TITLE>");
    response_ -> addBody("<BODY><P>The server could not fulfill");
    response_ -> addBody("your request because the resource specified");
    response_ -> addBody("is unavailable or nonexistent.");
    response_ -> addBody("</BODY></HTML>");

    response_ -> send();
}

void HttpRequest::badRequest()
{
    response_ -> addBody("<HTML><TITLE>Bad Request</TITLE>");

    response_ -> addBody("<BODY><P>The server could not fulfill");
    response_ -> addBody("your request because the resource specified");
    response_ -> addBody("is unavailable or nonexistent.");
    response_ -> addBody("</BODY></HTML>");

    response_ -> send();
}

void HttpRequest::initResponse(int code)
{
    response_ = new HttpResponse(this, code);
}

string HttpRequest::getQuery() const
{
    return parser_ -> getQuery();
}

HttpParser* HttpRequest::getParser()
{
    return parser_;
}

HttpResponse* HttpRequest::getResponse()
{
    return response_;
}

};