// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _HTTP_CONN_H
#define _HTTP_CONN_H

#include "MSGHandler.h"
using namespace eventserver;

namespace http
{

class HttpServer;
class HttpClient;
class HttpResponse;
class HttpParser;

class HttpRequest : public MSGHandler
{
public:
    /// Constructor, for the server
    HttpRequest(HttpServer *server, EventLoop *loop, Socket sock);

    /// Constructor, for the client
    HttpRequest(HttpClient *client, string url, EventLoop *loop, Socket sock);

    /// Destructor
    virtual ~HttpRequest();

public:
    /// Invoked when a connection we try either succeeds or fails.
    /// Helper for the client
    /// Only   for the client
    virtual void connected();

    /// Invoked when a message is received.
    /// Server will processed the message
    /// Client will invoke the callback
    virtual void received(STATUS status, Buffer &receivedBuff);

    /// Invoked when a msg has been sent
    /// Only for the server
    virtual void sent(STATUS status, int len, int targetLen);

public:
    /// For the server

    /// send 404
    void    notFound();

    /// 400
    void    badRequest();

    /// Get query string
    string  getQuery() const;

    /// Init the response
    void    initResponse(int code);

    /// Get the Response
    HttpResponse* getResponse();

    /// Get the parser
    HttpParser* getParser();

private:
    /**
    ** For the server
    **/

    /// The server
    HttpServer  *server_;

    /// Each request will have a response
    HttpResponse *response_;

    /// bool first, the first read
    bool          first_;

public:
    /**
    ** For the client
    **/

    /// Return the url
    string getUrl() const
    {
        return url_;
    }

    /// Return the errcode_
    int    getCode() const
    {
        return errcode_;
    }

public:
    /// placeholder
    virtual void RegisterVariable(void *arg) { }

private:
    /**
    ** For the client
    **/

    /// The client
    HttpClient  *client_;

    string       url_;

private:
    /**
    ** common variable
    **/
    /// The errcode_ represent the type of error of parameters
    int          errcode_;

    /// The parser result
    HttpParser   *parser_;
};

};

#endif