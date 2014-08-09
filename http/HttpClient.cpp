// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpClient.h"
#include "HttpParser.h"

namespace http
{

HttpClient::HttpClient()
{
}

HttpClient::~HttpClient()
{
}

void HttpClient::start()
{
    pool_.subrun();
}

void HttpClient::wait()
{
    pool_.subjoin();
}

void HttpClient::stop()
{
    pool_.stop();
}

bool HttpClient::request(const string &url, Callback get, Callback error, void *arg)
{
    if(calls_.find(url) != calls_.end())
    {
        Pair pair = calls_[url];
        if(get != pair.first.first || error != pair.first.second
                || arg != pair.second)
        {
            return false;
        }
    }
    else
    {
        calls_[url] =  make_pair(make_pair(get, error), arg);
    }

    /// TBD, parse the url and find out the ip/port
    string host, qstr;
    int port;

    assert(HttpParser::parseURL(url, host, port, qstr) == true);

    NetAddress svrAddr(host, port);

    Socket sock(AF_INET, SOCK_STREAM);

    /// Need further optimization
    if(sock.connect(&svrAddr) == false)
    {
        INFO << sock.status() ;
        assert(0);
    }

    /// Don't need to de-locate it
    int id;
    new HttpRequest(this, url, pool_.getRandomLoop(id), sock);
}


void HttpClient::process(HttpRequest *req)
{
    string     url = req -> getUrl();
    int    errcode = req -> getCode();

    assert(calls_.find(url) != calls_.end());

    Callback   get = calls_[url].first.first;
    Callback error = calls_[url].first.second;
    void      *arg = calls_[url].second;

    if(errcode != 0)
    {
        /// error in reply
        error(req, arg);
    }
    else
    {
        /// everything is ok
        get(req, arg);
    }

}

};
