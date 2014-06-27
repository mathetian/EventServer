// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Buffer.h"
using namespace utils;

namespace sealedserver
{

class HttpParser{
public:
	/// Parse the head & body
    /// Currently, only support get
    /// That means we won't support post 
    /// In this version
    bool parse(Buffer &receivedBuff);

    /// Get the url
    string getQuery() const;

private:
    /// Parse the first line
    bool parseFirstLine(const string &str, int &index);
    
    /// Parse the header except the first line
    bool parseHeader(string str);

    /// Parse each line of the header expect the first line
    bool parseLine(string str);

    /// Parse the parameters in the url
    bool parseURL();
    bool decode(string para);
    bool parsekv(string str);
    
    /// Check the information
    int  is_valid_http_method(const char *s);
    bool check();

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
};

};