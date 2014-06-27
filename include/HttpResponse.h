// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Buffer.h"
#include "Noncopyable.h"
using namespace utils;

#include "HttpRequest.h"

namespace sealedserver
{

class HttpResponse : public Noncopyable
{
public:
	/// Constructor
	HttpResponse(HttpRequest*, int code);
	
	/// Deconstructor
	virtual ~HttpResponse();

public:
	/// The send operation
	void send();

public:
	/// Add header(each line will constite with key/value)
	void addHeader(const string &key, const string &value);

	/// Add body
	void addbody(const string &body);

private:	
	string       header_;
	string       body_;
	HttpRequest *request_;
};

};
