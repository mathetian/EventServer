// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _HTTP_SERV_H
#define _HTTP_SERV_H

#include "Noncopyable.h"
using namespace utils;

#include "HttpAcceptor.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

#include "EventPool.h"

namespace sealedserver
{

class HttpServer : public Noncopyable
{
    typedef void (*Callback) (HttpRequest*, HttpResponse*, void*);
    typedef pair<Callback, void*> Pair;
    typedef map<string, Pair> Callbacks;

public:
    /// Constructor
    HttpServer(int port);

    /// Constructor
    /// place is short for placeholder
    /// place is used in inherited class
    HttpServer(int port, int place);

    /// Destructor
    virtual ~HttpServer();

public:
    void start();
    void stop();
    void add(const string &url, Callback callback, void *arg);
    void error(Callback callback, void *arg);

public:
    bool process(HttpRequest *conn);

protected:
    EventPool pool_;
    Callbacks calls_;
    int       port_;
    Pair      error_;
    bool      errflag_;
    HttpAcceptor<HttpRequest> *acceptor_;

};

};

#endif