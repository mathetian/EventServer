#include "../utils/Utils.h"
#include "../utils/Log.h"

#include <string>
using namespace std;

class A
{
public: 
	string as_string() const
	{
		return "Class A";
	}
};
TO_STRING(A);

int main()
{
	DEBUG << "hello, begin";
	int a = 3; bool b = true; char c = 'G'; uint64_t d = 1ull << 51;
	DEBUG << a << b << c <<" "<<d;
	A e;
	DEBUG << e;
	return 0;
}