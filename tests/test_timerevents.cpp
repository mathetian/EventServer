// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Timer.h"
#include "Tester.h"
#include "TimerEvents.h"
using namespace utils;

class A
{

};

class B
{
public:
    void call_1_f()
    {
        cout << "call_1" << endl;
    }

    void call_2_f()
    {
        cout << "call_2" << endl;
    }

};

TEST(A, Plain)
{
    TimerEvents heap;
    Timer cur_1 = Timer::now();
    Timer cur_2(10000);
    B b;
    Callback<void> call_1(&b, &B::call_1_f);
    Callback<void> call_2(&b, &B::call_2_f);

    heap.add(call_1, cur_1);
    heap.add(call_2, cur_2);

    while(heap.empty() == false)
    {
        heap.top().call();
        heap.pop();
    }
}

int main()
{
    RunAllTests();
    return 0;
}