// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _HTTP_PARSER_H
#define _HTTP_PARSER_H

#include "Buffer.h"
using namespace utils;

namespace http
{

class HttpParser
{
    typedef map<string, string> Header;
    typedef map<string, string> Params;
public:
    typedef enum {REQUEST, RESPONSE} ParseType;

public:
    /// There are two packages that need to be resolved
    ///
    /// @param type, 0 for the request(parse the request package)
    ///            , 1 for the response(parse the response package)
    HttpParser(ParseType type);

public:
    /// Parse the head & body
    /// Currently, only support get
    /// That means we won't support post
    /// In this version
    bool parse(Buffer &receivedBuff);

private:
    /// Parse the first line
    bool parseFirstLine(const string &str, int &index);

    /// Parse the header except the first line
    bool parseHeader(string str, int &index);

    /// Parse each line of the header expect the first line
    bool parseLine(string str);

    /// Parse the parameters in the url
    bool parseURL();
    bool decode(string para);
    bool parsekv(string str);

    /// Check the information
    int  is_valid_http_method(const char *s);
    bool check();

public:
    /// Get Method
    string getMethod()  const
    {
        return method_;
    }

    /// Get URL
    string getUrl() const
    {
        return url_;
    }

    /// Get Version
    string getVersion() const
    {
        return version_;
    }

    /// Get Header
    Header getHeader() const
    {
        return header_;
    }

    /// Get the url
    string getQuery() const
    {
        return querystring_;
    }

    /// Get the params
    Params getParams() const
    {
        return params_;
    }

    /// Get the origin string
    string getOrigin() const
    {
        return origin_;
    }

public:
    /// A helper function for parsing the url
    ///
    /// @param url, the url
    ///       host, the parsed ip(result)
    ///       port, the port(usually 80)
    ///       qstr, the query string
    /// @return false, failed
    ///         true , successful
    ///
    /// We can only support two types
    ///           119.75.218.77/s?wd=a
    ///Or
    ///           119.75.218.77:80/s?wd=a
    ///
    /// The first type means that the port is the default port, which means 80
    /// you can't add http* or others in this url
    /// Also, you can give `119.75.218.77/` as a parameters
    static bool parseURL(const string &url, string &host, int &port, string &qstr);

private:
    /// The first line
    string       method_;
    string       url_;
    string       version_;

    /// The result of parsed url_
    string       querystring_;
    Params       params_;

    /// The header
    Header       header_;

    /// The body. Only supported empty
    string       body_;

    /// string origin
    string       origin_;

    /// the request/response
    ParseType   type_;
};

};

#endif