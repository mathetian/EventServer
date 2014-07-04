// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpServer.h"

namespace sealedserver
{

HttpServer::HttpServer(int port, int portnum) : port_(port), errflag_(false), portnum_(portnum)
{
    acceptors_ = vector<HttpAcceptor<HttpRequest>*>(portnum);

    for(int i = 0; i < portnum ;i++)
        acceptors_[i] = new HttpAcceptor<HttpRequest>(this, pool_.getRandomLoop(), port + i);
}

HttpServer::HttpServer(int port, int portnum, int place) : port_(port), errflag_(false), portnum_(portnum)
{

}

HttpServer::~HttpServer()
{
    pool_.stop();

    for(int i = 0; i < portnum_ ;i++)
    {
        delete acceptors_[i];
        acceptors_[i] = NULL;
    }
}

void HttpServer::start()
{
    pool_.run();
}

void HttpServer::stop()
{
    pool_.stop();
}

void HttpServer::add(const string &url, Callback callback, void *arg)
{
    calls_[url] = make_pair(callback, arg);
}

void HttpServer::error(Callback callback, void *arg)
{
    error_   = make_pair(callback, arg);
    errflag_ = true;
}

bool HttpServer::process(HttpRequest *req)
{
    string query = req -> getQuery();

    if(calls_.find(query) ==  calls_.end())
    {
        req -> initResponse(404);

        if(errflag_)
        {
            Callback callback = error_.first;
            void    *arg      = error_.second;

            callback(req, req -> getResponse(), arg);
        }
        else
        {
            req -> notFound();
        }
    }
    else
    {
        req -> initResponse(200);

        Callback callback = calls_[query].first;
        void    *arg      = calls_[query].second;
        callback(req, req -> getResponse(),arg);
    }
}

};