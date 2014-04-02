#include <string>
#include <iostream>
using namespace std;

#include "Buffer.h"
using namespace utils;

void TestBuffer(const Buffer buf)
{
    cout<<"len2:"<<buf.length()<<endl;
}

int main()
{
    Buffer buf("hello world");
    cout<<"len1:"<<buf.length()<<endl;
    TestBuffer(buf);
    return 0;
}