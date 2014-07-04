// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "C.h"
using namespace utils;

#include "HttpParser.h"

namespace sealedserver
{

HttpParser::HttpParser(HttpParser::ParseType type) : type_(type)
{

}

bool HttpParser::parse(Buffer &receivedBuff)
{
    string str = (string)receivedBuff;

    origin_ = str;

    int index;
    bool flag;
    flag = parseFirstLine(str, index);

    if(flag == false)
    {
        return false;
    }

    flag = check();
    if(flag == false)
    {
        return false;
    }

    flag = parseURL();
    if(flag == false)
    {
        return false;
    }

    str  = str.substr(index);
    flag = parseHeader(str, index);

    if(flag == false)
        return false;

    assert(index < str.size());

    body_ = str.substr(index);

    return true;
}

bool HttpParser::parseFirstLine(const string &str, int &index)
{
    int i = 0, len = str.size(), j;

    /// skip space
    for(; i < len && str.at(i) == ' '; i++);
    if(i >= len) return false;

    /// Find the first space
    for(j = i + 1; j < len && str.at(j) != ' '; j++);
    if(j >= len) return false;

    method_ = str.substr(i, j - i);

    /// skip space
    for(i = j; i < len && str.at(i) == ' '; i++);
    if(i >= len) return false;

    /// Find the first space
    for(j = i + 1; j < len && str.at(j) != ' '; j++);
    if(j >= len) return false;

    url_ = str.substr(i, j - i);

    /// skip space
    for(i = j; i < len && str.at(i) == ' '; i++);
    if(i >= len) return false;

    /// Find the `\r` space
    for(j = i + 1; j < len && str.at(j) != '\r'; j++);
    if(j >= len - 1 || str.at(j + 1) != '\n') return false;

    version_ = str.substr(i, j - i);

    index = j + 2;

    return true;
}

bool HttpParser::parseHeader(string str, int &rindex)
{
    bool flag = true;
    rindex = 0;
    while(str.size() != 0)
    {
        int index = str.find("\r\n");
        if(index == -1)
        {
            return false;
        }

        if(index == 0) break;

        flag = parseLine(str.substr(0, index));

        if(flag == false)
        {
            return false;
        }

        rindex += index + 2;
        str = str.substr(index + 2);
    }

    return true;
}

bool HttpParser::parseLine(string str)
{
    int index = str.find(": ");

    if(index == -1 || index == 0 || index >= str.size() - 2)
        return false;

    header_[str.substr(0,index)] = str.substr(index + 2);
}

/// only support get
int HttpParser::is_valid_http_method(const char *s)
{
    return !strcmp(s, "GET");
}

bool HttpParser::check()
{
    if(type_ == REQUEST)
    {
        if(is_valid_http_method(method_.c_str()) == false)
            return false;

        if (memcmp(version_.c_str(), "HTTP/", 5) != 0)
            return false;
    }
    /// to reduce the bound
    /// we omit the check of response package

    return true;
}

bool HttpParser::parseURL()
{
    int index = url_.find("?");
    bool flag = true;

    if(index == -1)
        querystring_ = url_;
    else
    {
        querystring_ = url_.substr(0, index);
        flag = decode(url_.substr(index + 1));
    }

    return flag;
}

#define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')

bool HttpParser::decode(string para)
{
    int i, a, b;
    string decodestr;
    int len = para.size();

    for(i = 0; i < len; i++)
    {
        if(para.at(i) == '%' && i < para.size() -2 &&
                isxdigit(para.at(i+1)) && isxdigit(para.at(i+2)))
        {
            a = tolower(para.at(i + 1));
            b = tolower(para.at(i + 2));
            decodestr.push_back((char) ((HEXTOI(a) << 4) | HEXTOI(b)));
            i += 2;
        }
        else if(para.at(i) == '+')
        {
            decodestr.push_back(' ');
        }
        else
        {
            decodestr.push_back(para.at(i));
        }
    }

    bool flag = true;

    while(flag == true)
    {
        int index = decodestr.find("&");
        if(index == -1)
        {
            flag = parsekv(decodestr);
            break;
        }
        flag = parsekv(decodestr.substr(0, index));
        decodestr = decodestr.substr(index + 1);
    }

    return flag;
}

bool HttpParser::parsekv(string str)
{
    int index = str.find("=");
    if(index == -1 || index == 0)
        return false;
    params_[str.substr(0, index)] = str.substr(index + 1);

    return true;
}

bool HttpParser::parseURL(const string &url, string &host, int &port, string &qstr)
{
    int index1 = url.find("/");
    int index2 = url.find(":");

    if(index1 == -1)
    {
        /// That means qstr == empty
        qstr = "/";

        if(index2 == -1)
        {
            host = url;
            port = 80;
        }
        else
        {
            host = url.substr(0, index2);
            port = to_int(url.substr(index2 + 1));
        }
    }
    else
    {
        /// That means qstr != empty
        qstr = "/" + url.substr(index1 + 1);

        if(index2 == -1 || index2 > index1)
        {
            /// That means without port
            host = url.substr(0, index1);
            port = 80;
        }
        else
        {
            host = url.substr(0, index2);
            port = to_int(url.substr(index2 + 1, index1 - index2 - 1));
        }
    }

    return true;
}

};