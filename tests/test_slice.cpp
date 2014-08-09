// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Slice.h"
#include "Tester.h"
using namespace utils;

class A { };

TEST(A, Assignment)
{
    string str = "hello, body";
    Slice slice = str;
    ASSERT_EQ((string)slice, "hello, body");
}

TEST(A, ConstScope)
{
    Slice slice;

    {
        string str = "hello, body";
        slice = str;
    }

    ASSERT_NE(slice.str(), "hello, body");
    ASSERT_EQ((string)slice, "hello, body");
}

TEST(A, HeapScope)
{
    Slice slice;

    {
        char *data = new char[5];
        memset(data, 0, 5);
        strcpy(data, "hell");
        slice = Slice(data, 5);
        delete [] data;
        data = NULL;
    }
    ASSERT_EQ((string)slice, "");
}

int main()
{
    RunAllTests();
    return 0;
}