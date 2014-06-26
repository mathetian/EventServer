// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpServer.h"

namespace sealedserver
{

HttpServer::HttpServer(int port) : port_(port), errflag_(false)
{
	acceptor_ = new HttpAcceptor<HttpConnection>(pool_.getRandomLoop(), port);
}

HttpServer::~HttpServer()
{
	pool_.stop();

	delete acceptor_;
	acceptor_ = NULL;
}

void HttpServer::start()
{
	pool_.run();
}

void HttpServer::add(const string &url, Callback callback, void *arg)
{
	calls_[url] = make_pair(callback, arg);
}

void HttpServer::adderror(Callback callback, void *arg)
{
	error_   = make_pair(callback, arg);
	errflag_ = true;
}

bool HttpServer::process(HttpConnection *conn)
{
	Header header = conn -> getHeader();
	assert(header.find("url") != header.end());

	string url = header["url"];
	if(calls_.find(url) ==  calls_.end())
	{
		if(errflag_)
		{
			Callback callback = error_.first;
			void    *arg      = error_.second;

			callback(conn, arg);
		}
		else
		{
			conn -> notfound();
		}
	}
	else
	{
		Callback callback = calls_[url].first;
		void    *arg      = calls_[url].second;
		callback(conn, arg);
	}
}

};