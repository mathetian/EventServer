// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "C.h"
#include "Tester.h"
using namespace utils;

class A
{
public:
    string as_string() const { return "Class A"; }
};

TO_STRING(A);

class B
{
    int v;
public:
    B() : v(3) { }
    string as_string() const { return to_string(v); }
};

TO_STRING(B);

TEST(A, TestA)
{
    A a;
    ASSERT_EQ(to_string(a), "Class A");
}

TEST(A, TestB)
{
    B b;
    ASSERT_EQ(to_string(b), "3");
}

int main()
{
    RunAllTests();
    
    return 0;
}