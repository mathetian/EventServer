#include <iostream>
#include <string>
using namespace std;

#include "../utils/Slice.h"

int main()
{
	Slice slice;
	{
		string str = "hello, body";
		slice = str;
		cout<<slice<<endl;

		Slice slice2(str);
		cout<<(string)slice2<<endl;
	
		char data[] = {'a', 'b', 'c', 0};
		slice = Slice(data, 4);
	}
	cout<<(string)slice<<endl;
	return 0;
}