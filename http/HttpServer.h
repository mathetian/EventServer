// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _HTTP_SERV_H
#define _HTTP_SERV_H

#include "Noncopyable.h"
using namespace utils;

#include "EventPool.h"
using namespace sealedserver;

#include "HttpAcceptor.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace http
{

class HttpServer : public Noncopyable
{
    typedef void (*Callback) (HttpRequest*, HttpResponse*, void*);
    typedef pair<Callback, void*> Pair;
    typedef map<string, Pair> Callbacks;

public:
    /// Constructor
    HttpServer(int port, int portnum);

    /// Constructor
    /// place is short for placeholder
    /// place is used in inherited class
    ///
    /// For extendible ClassEs
    HttpServer(int port, int portnum, int place);

    /// Destructor
    virtual ~HttpServer();

public:
    /// Start the server
    /// To be special, it is used to start the `eventloop`
    void start();

    /// Stop the server
    /// To be special, it is used to stop the `eventloop`
    void stop();

    /// Add callback for `url`
    void add(const string &url, Callback callback, void *arg);

    /// Set the error callback
    void error(Callback callback, void *arg);

public:
    /// Process the `request`
    bool process(HttpRequest *req);

protected:
    EventPool pool_;
    Callbacks calls_;
    int       port_;
    Pair      error_;
    bool      errflag_;

    int       portnum_;
    vector<HttpAcceptor<HttpRequest>*> acceptors_;
};

};

#endif