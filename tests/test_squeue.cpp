// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "SQueue.h"
#include "Tester.h"
#include "Callback.h"
using namespace utils;

class A
{
public:
    const char* a()
    {
        return "A::a";
    }
};

class B
{
public:
    const char* b()
    {
        return "B::b";
    }
};

TEST(A, Plain)
{
    SQueue<Callback<const char*> * > squeue;

    A a1;
    B b1;
    Callback<const char*> call1(&a1, &A::a);
    Callback<const char*> call2(&b1, &B::b);
    squeue.push(&call1);
    squeue.push(&call2);
    ASSERT_EQ(squeue.empty(), false);

    Callback<const char*> *call3 = squeue.get();
    Callback<const char*> *call4 = squeue.get();

    ASSERT_EQ((*call3)(), "A::a");
    ASSERT_EQ((*call4)(), "B::b");
    ASSERT_EQ(squeue.empty(), true);
}

int main()
{
    RunAllTests();
    return 0;
}