// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
using namespace sealedserver;

#define Port 8080

HttpServer server(Port);

string concat(string key, string value)
{
    return key + ": " + value + "\r\n";
}

void sign_handler(HttpRequest *req, HttpResponse *rep, void *arg)
{
    rep -> addHeader("Content-Type", "text/plain");

    HttpParser *parser = req -> getParser();

    string origin = parser -> getOrigin();

    rep -> addBody(origin);

    rep -> addBody("\n");

    map<string, string> params = parser -> getParams();
    map<string, string>::iterator iter = params.begin();

    for(; iter != params.end(); iter++)
    {
        rep -> addBody(concat((*iter).first, (*iter).second));
    }

    rep -> send();
}

void pub_handler(HttpRequest *req, HttpResponse *rep, void *arg)
{

}

void sub_handler(HttpRequest *req, HttpResponse *rep, void *arg)
{

}

void error_handler(HttpRequest *req, HttpResponse *rep, void *arg)
{
    rep -> addHeader("Content-Type", "text/plain");

    HttpParser *parser = req -> getParser();

    rep -> addBody(concat("method", parser -> getMethod()));
    rep -> addBody(concat("version", parser -> getVersion()));
    rep -> addBody(concat("url", parser -> getUrl()));

    map<string, string> header = parser -> getHeader();

    map<string, string>::iterator iter = header.begin();

    for(; iter != header.end(); iter++)
    {
        rep -> addBody(concat((*iter).first, (*iter).second));
    }

    rep -> addBody("\n");

    map<string, string> params = parser -> getParams();
    iter = params.begin();

    for(; iter != params.end(); iter++)
    {
        rep -> addBody(concat((*iter).first, (*iter).second));
    }

    rep -> send();
}

/// Signal Stop the server
void signalStop(int)
{
    INFO << "Stop running...by manually";
    server.stop();
}

/// Change the configure
int setlimit(int num_pipes)
{
    struct rlimit rl;
    rl.rlim_cur = rl.rlim_max = num_pipes * 2 + 50;
    if (::setrlimit(RLIMIT_NOFILE, &rl) == -1)
    {
        fprintf(stderr, "setrlimit error: %s", strerror(errno));
        return 1;
    }
}

int main()
{
    Log::setLevel(Log::debug);

    ::signal(SIGINT, signalStop);
    setlimit(100000);
    errno = 0;

    server.add("/sign", sign_handler, NULL);
    server.add("/pub",  pub_handler, NULL);
    server.add("/sub",  sub_handler, NULL);
    server.error(error_handler, NULL);

    INFO << "server starting" ;
    server.start();

    INFO << "server stopping" ;
    return 0;
}