// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "C.h"
using namespace utils;

#include "HttpResponse.h"

namespace http
{

static const char *status_code_to_str(int status_code)
{
    switch (status_code)
    {
    case 200:
        return "OK";
    case 201:
        return "Created";
    case 204:
        return "No Content";
    case 301:
        return "Moved Permanently";
    case 302:
        return "Found";
    case 304:
        return "Not Modified";
    case 400:
        return "Bad Request";
    case 403:
        return "Forbidden";
    case 404:
        return "Not Found";
    case 405:
        return "Method Not Allowed";
    case 409:
        return "Conflict";
    case 411:
        return "Length Required";
    case 413:
        return "Request Entity Too Large";
    case 415:
        return "Unsupported Media Type";
    case 423:
        return "Locked";
    case 500:
        return "Server Error";
    case 501:
        return "Not Implemented";
    default:
        return "Server Error";
    }
}

/// For the server
HttpResponse::HttpResponse(HttpRequest *request, int code) : request_(request)
{
    stringstream ss;
    ss << "HTTP/1.1 " << code << " " << status_code_to_str(code) << "\r\n";

    header_ = ss.str();
}

/// For the client
HttpResponse::HttpResponse(HttpRequest *request) : request_(request)
{
    string url = request -> getUrl();

    string host, qstr;
    int port;

    assert(HttpParser::parseURL(url, host, port, qstr) == true);

    if(port != 80)
    {
        stringstream ss1;
        ss1 << host << ":" << port;
        host = ss1.str();
    }

    stringstream ss2;
    ss2 << "GET " << qstr << " " << "HTTP/1.1" << "\r\n";

    header_ = ss2.str();

    addHeader("Host", host);
    addHeader("User-Agent", "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:24.0) Gecko/20100101 Firefox/24.0");
    addHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    addHeader("Accept-Language", "en-US,en;q=0.5");
    addHeader("Accept-Encoding", "gzip, deflate");
    addHeader("Connection", "keep-alive");
}

HttpResponse::~HttpResponse()
{
}

void HttpResponse::send()
{
    string content = header_ + "\r\n" + body_;
    request_ -> send(content);
}

void HttpResponse::addHeader(const string &key, const string &value)
{
    stringstream ss;
    ss << key << ": " << value << "\r\n";

    header_ += ss.str();
}

void HttpResponse::addBody(const string &body)
{
    body_ += body;
}

};