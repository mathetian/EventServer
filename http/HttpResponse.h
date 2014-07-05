// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _HTTP_RESPONSE_H
#define _HTTP_RESPONSE_H

#include "Noncopyable.h"
using namespace utils;

#include "HttpParser.h"
#include "HttpRequest.h"

namespace http
{

class HttpResponse : public Noncopyable
{
public:
    /// Constructor, for the request
    HttpResponse(HttpRequest*, int code);

    /// Constructor
    HttpResponse(HttpRequest*);

    /// Deconstructor
    virtual ~HttpResponse();

public:
    /// The send operation
    void send();

public:
    /// Add header(each line will constite with key/value)
    void addHeader(const string &key, const string &value);

    /// Add body
    void addBody(const string &body);

private:
    string       header_;
    string       body_;
    HttpRequest *request_;
};

};

#endif