// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "C.h"
#include "Log.h"
#include "Tester.h"
using namespace utils;

class A 
{ 
public:
    string extract(string str)
    {
    	int index = str.find("]");
    	assert(index != -1);

    	int len = str.size();

    	return str.substr(index + 2, len - index - 3);
    }
};

class B
{
public:
	string as_string() const
    {
    	return "Hello, body";
    }
};

TO_STRING(B);

TEST(A, Plain)
{
	int a = 3;
	DEBUG << a;
	INFO << 31 << " " << 31;
}

TEST(A, Stream)
{
	stringstream ss; string str;
	Log::setStream(&ss);

	int a = 3;
	DEBUG << a;
	str = extract(ss.str());
	
	ASSERT_EQ(str, "31");

	ss.clear();
	
	INFO << 31 << " " << 31;
	str = extract(ss.str());
	
	ASSERT_EQ(str, "31 31");

	/**
	** Need set it back manually
	**/
	Log::setStream(&cout);
}

TEST(A, Object)
{
	stringstream ss; string str;
	Log::setStream(&ss);

	B b;
	DEBUG << b;

	str = extract(ss.str());
	ASSERT_EQ(str, "Hello, body");

	/**
	** Need set it back manually
	**/
	Log::setStream(&cout);
}

int main()
{
	/**
	** set the level of logging manually
	**/
	Log::setLevel(Log::debug);
    
    RunAllTests();
    return 0;
}