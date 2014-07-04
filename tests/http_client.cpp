// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpClient.h"

#include "HttpParser.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
using namespace sealedserver;

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
    client.request("202.120.58.161", get, error, NULL);

    /// yulongti.info/?p=2761
    client.request("128.199.204.82:80/?p=2761", get, error, NULL);

    client.wait();

    return 0;
}