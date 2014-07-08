// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Tester.h"
#include "Thread.h"
using namespace utils;

class A
{
public:
    void Func1()
    {
        ScopeMutex scope(&lock);
        printf("Enter Func1, %lld\n", (long long)Thread::getIDType());
        sleep(1);
        Func2();
    }

    void Func2()
    {
        ScopeMutex scope(&lock);
        printf("Enter Func2, %lld\n", (long long)Thread::getIDType());
        sleep(1);
        Func3();
    }

    void Func3()
    {
        ScopeMutex scope(&lock);
        printf("Enter Func3, %lld\n", (long long)Thread::getIDType());
    }

private:
    ReentrantLock lock;
};

TEST(A, ReentrantLock)
{
    Func1();
}

A a;

void* Call(void *data)
{
    a.Func1();
}

/**
** The target is that there are three consective number
**/
TEST(A, MultiReentrantLock)
{
    A a;
    Thread ** threads = new Thread*[4];
    for(int i=0; i<4; i++)
        threads[i] = new Thread(Call, NULL);
    for(int i=0; i<4; i++) threads[i]->run();
    for(int i=0; i<4; i++) threads[i]->join();
}

int main()
{
    RunAllTests();
    return 0;
}