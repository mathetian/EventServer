// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Log.h"
#include "Status.h"
#include "Tester.h"
using namespace utils;

class A { };

TEST(A, ok)
{
    Status status;
    ASSERT_EQ(status, true);
}

TEST(A, error)
{
    int fd = open("hello.txt", O_RDONLY);
    Status status(strerror(errno));
    ASSERT_EQ(status, false);
    WARN << status ;
}

int main()
{
    RunAllTests();
    return 0;
}