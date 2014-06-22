// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Tester.h"
using namespace utils;

class A
{
public:
    friend bool operator == (const A &a1, const A &a2);

    friend ostream& operator << (ostream &os, const A &a1);
};

bool operator == (const A &a1, const A &a2)
{
    return true;
}

ostream& operator << (ostream &os, const A &a1)
{
    os << "Address: " << &a1;
    return os;
}

TEST(A, Plain)
{
    ASSERT_EQ(1, 1);
    A a, b;
    ASSERT_EQ(a, b);
}

TEST(A, String)
{
    string str1 = "hello";
    const char *str2 = "hello";
    ASSERT_EQ(str1, "hello");
    ASSERT_EQ(str2, "hello");
    ASSERT_EQ(str2, str1);
    const char *str3 = "hello";
    ASSERT_EQ(str2, str3);
}

int main()
{
    RunAllTests();
    return 0;
}