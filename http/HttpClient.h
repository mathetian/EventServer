// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Noncopyable.h"
using namespace utils;

#include "EventPool.h"
using namespace sealedserver;

#include "HttpAcceptor.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace http
{

class HttpClient : public Noncopyable
{
    typedef void (*Callback) (HttpRequest*, void*);
    typedef void (*Error) (HttpRequest*, void*);
    typedef pair<pair<Callback, Callback>, void*> Pair;

    /// each url with has  a Pair
    /// That means, we don't support that
    ///
    /// client_.request(url, get1, error1, arg1);
    /// client_.request(url, get2, error2, arg2);
    ///
    /// Also, we won't clean the url we have requests
    typedef map<string, Pair> Callbacks;

public:
    /// Constructor
    HttpClient();

    /// Destructor
    virtual ~HttpClient();

public:
    /// start the client(start the loop)
    void start();

    /// wait util all request have been processed
    void wait();

    /// cancal all request instantly
    void stop();

public:
    /// request with the url, get ,error, arg
    /// Async
    ///
    /// @param url, the url(the key of calls)
    ///        get, when successful call this function
    ///		 error, when failed call this function
    ///        arg, extra args(usually empty)
    ///
    /// @return true , request successful
    /// @return false, request failed
    bool request(const string &url, Callback get, Callback error, void *arg);

public:
    void process(HttpRequest *req);

private:
    EventPool pool_;
    Callbacks calls_;
    int       port_;
};

};
