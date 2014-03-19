#include <iostream>
#include <string>
using namespace std;

#include "../utils/Buffer.h"

void TestBuffer(const Buffer buf)
{
	cout<<"len:"<<buf.length()<<endl;
}

int main()
{
	Buffer buf("hello world");
	cout<<"len1:"<<buf.length()<<endl;
	TestBuffer(buf);
	return 0;
}