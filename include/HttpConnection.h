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
    typedef map<string, string> Params;
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
    /// send 404
    void    notFound();
    void    badRequest();
    string  getQuery() const;

private:
    /// Parse the head & body
    /// Currently, only support get
    /// That means we won't support post 
    /// In this version
    void parse(Buffer &receivedBuff);
    
    /// Parse the first line
    bool parseFirstLine(const string &str, int &index);
    
    /// Parse the header except the first line
    bool parseHeader(string str);

    /// Parse each line of the header expect the first line
    bool parseLine(string str);

    /// Parse the parameters in the url
    bool parseURL();
    bool decode(string para);
    bool parsekv(string str);
    
    /// Check the information
    int is_valid_http_method(const char *s);
    bool check();

private:
    Header       header_;
    string       body_;
    HttpServer  *server_;
    int          errcode_;
    string       method_;
    string       url_;
    string       querystring_;
    string       version_;
    Params       params_;
};

};

#endif