// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "HttpParser.h"
using namespace sealedserver;

#include "Tester.h"
using namespace utils;

class A { };

TEST(A, ParseURL1)
{
    string host, qstr;
    int port;
    HttpParser::parseURL("119.75.218.77/s?wd=a", host, port, qstr);

    ASSERT_EQ(host, "119.75.218.77");
    ASSERT_EQ(qstr, "/s?wd=a");
    ASSERT_EQ(port, 80);
}

TEST(A, ParseURL2)
{
    string host, qstr;
    int port;
    HttpParser::parseURL("119.75.218.77:81/s?wd=a", host, port, qstr);

    ASSERT_EQ(host, "119.75.218.77");
    ASSERT_EQ(qstr, "/s?wd=a");
    ASSERT_EQ(port, 81);
}

TEST(A, ParseURL3)
{
    string host, qstr;
    int port;
    HttpParser::parseURL("119.75.218.77:81", host, port, qstr);

    ASSERT_EQ(host, "119.75.218.77");
    ASSERT_EQ(qstr, "/");
    ASSERT_EQ(port, 81);
}

int main()
{
    RunAllTests();
    return 0;
}