// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpClient.h"

namespace sealedserver
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
				|| arg != pair.second){
			return false;
		}
	}
	else
	{
		calls_[url] =  make_pair(make_pair(get, error), arg);
	}

	/// TBD, parse the url and find out the ip/port
	
	NetAddress svrAddr(string("202.120.58.161"), 80);

    Socket sock(AF_INET, SOCK_STREAM);
    sock.connect(&svrAddr);
//	Socket sock;

    /// Don't need to de-locate it
    HttpRequest *request = new HttpRequest(this, url, pool_.getRandomLoop(), sock);
}


void HttpClient::process(HttpRequest *req)
{
	string     url = req -> getUrl();
	int    errcode = req -> getCode();

	assert(calls_.find(url) != calls_.end());

	Callback   get = calls_[url].first.first;
	Callback error = calls_[url].first.second;
	void      *arg = calls_[url].second;
	
	if(errcode != 0){
		/// error in reply
		error(req, arg);	
	}
	else {
		/// everything is ok
		get(req, arg);
	}

}

};
