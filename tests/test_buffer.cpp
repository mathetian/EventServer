// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Buffer.h"
#include "Tester.h"
using namespace utils;

class A
{
    Buffer buf;
public:
    A() : buf("hello world") { }

    int size()
    {
        return buf.size();
    }
};

/**
** Test for size
**/
TEST(A, Size)
{
    ASSERT_EQ(size(), 11);
};

/**
** Test for counter
** Expect behavior: release/output/release/release
**/
TEST(A, Counter)
{
    Buffer buff;

    {
        Buffer buff1("hello world");
        buff = buff1;
    }

    cout << (string)buff << endl;
};

/**
** Test for counter
** Expect behavior: release/output/release/release
**/
TEST(A, SelfAlloc)
{
    Buffer buff;

    {
        char * str = new char[10];
        memset(str, 0, 10);
        strncpy(str, "great day", 9);

        Buffer buff1(str, true);
        buff = buff1;
    }

    cout << (string)buff << endl;
};

int main()
{
    RunAllTests();
    return 0;
}