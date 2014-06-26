// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpServer.h"
#include "HttpConnection.h"
using namespace sealedserver;

#define Port 8080

HttpServer server;

void sign_handler(HttpConnection * conn, void *arg)
{
}

void pub_handler(HttpConnection * conn, void *arg)
{
}

void sub_handler(HttpConnection * conn, void *arg)
{
}

void error_handler(HttpConnection * conn, void *arg)
{
}

int main()
{
	server.set("/sign", sign_handler, NULL);
	server.set("/pub",  pub_handler, NULL);
	server.set("/sub",  sub_handler, NULL);
	server.seterror(error_handler, NULL);

	INFO << "server starting" ;
	server.start(Port);

	INFO << "server stopping" ;
	return 0;
}