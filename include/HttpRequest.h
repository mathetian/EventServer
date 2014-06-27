// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _HTTP_CONN_H
#define _HTTP_CONN_H

#include "MsgHandler.h"
using namespace sealedserver;

namespace sealedserver
{

class HttpServer;
class HttpResponse;

class HttpRequest : public MSGHandler
{
    typedef map<string, string> Header;
    typedef map<string, string> Params;
public:
	/// Constructor
	HttpRequest(HttpServer *server, EventLoop *loop, Socket sock);
	
	/// Destructor
	virtual ~HttpRequest();

public:
	/// Invoked when a message is received.
    virtual void received(STATUS status, Buffer &receivedBuff);

    /// Invoked when a msg has been sent
    virtual void sent(STATUS status, int len, int targetLen);

public:
    /// send 404
    void    notFound();
    
    /// 400
    void    badRequest();

    /// Get query string
    string  getQuery() const;

    /// Init the response
    void    initResponse(int code);

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
    /// The first line
    string       method_;
    string       url_;
    string       version_;

    /// The result of parsed url_
    string       querystring_;
    Params       params_;

    /// The header
    Header       header_;   
    
    /// The body. Only supported empty
    string       body_;

    /// The server
    HttpServer  *server_;

    /// The errcode_ represent the type of error of parameters
    int          errcode_;

    /// Each request will have a response
    HttpResponse *response_;

    /// bool first, the first read
    bool          first_;
};

};

#endif