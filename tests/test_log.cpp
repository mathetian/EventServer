// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Log.h"
#include "Tester.h"
using namespace utils;

class A { };

class B
{
public:
};

TEST(A, Plain)
{
	int a = 3;
	DEBUG << a;
}

Test(A, Object)
{

}

int main()
{
    
    DEBUG << a;
    INFO << 31 << " " << 31;
    return 0;
}