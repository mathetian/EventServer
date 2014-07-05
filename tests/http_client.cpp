// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpClient.h"

#include "HttpParser.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
using namespace http;

#define CLINUM 100000
#define Port 8081
#define PortNum 5

HttpClient client;

void get(HttpRequest *req, void *arg)
{
    HttpParser *parser = req -> getParser();

    WARN << parser -> getOrigin() ;
}

void error(HttpRequest *req, void *arg)
{
    WARN << "error" ;
}

/// Signal Stop the server
void signalStop(int)
{
    INFO << "Stop running...by manually";
    client.stop();
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

    client.start();

    /// bbs.sjtu.edu.cn
    /// client.request("202.120.58.161", get, error, NULL);

    /// yulongti.info/?p=2761
    /// client.request("128.199.204.82/?p=2761", get, error, NULL);

    for(int i = 0; i < CLINUM; i++)
    {
        stringstream ss; int port = Port + (i % PortNum);
        ss << "127.0.0.1" << ":" << port << "/sub";

        client.request(ss.str(), get, error, NULL);
        usleep(5 * 1000);
    }

    client.wait();

    return 0;
}