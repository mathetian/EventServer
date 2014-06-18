// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Tester.h"
#include "Callback.h"
using namespace utils;

class A 
{ 

};

class B
{
public:
    void b()
    {
        printf("hello, enter function b\n");
    }

    ~B()
    {
        printf("release B\n");
    }
};

TEST(A, callfromb)
{
	B b1;
    Callback<void> call(&b1, &B::b);
    call();
}

TEST(A, equal)
{
    Callback<void> call;

    B b1;
    call =  Callback<void>(&b1, &B::b);
    call();
}

/**
** Should throw exception, bug
**/
TEST(A, null)
{
    Callback<void> call;
    
    {
        B b1;
        call =  Callback<void>(&b1, &B::b);
    }
    
    call();
}

int main()
{    
	RunAllTests();
    return 0;
}