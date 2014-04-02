#include <string>
using namespace std;

#include "Log.h"
#include "Utils.h"
using namespace utils;

class A
{
public:
    string as_string() const
    {
        return "Class A";
    }
};
TO_STRING(A);

namespace testspace
{
class B
{
    int v;
public:
    B() : v(3) { }
    string as_string() const
    {
        return to_string(v);
    }
};

TO_STRING(B);
};

using namespace testspace;

int main()
{
    DEBUG << "hello, begin";
    int a = 3;
    bool b = true;
    char c = 'G';
    uint64_t d = 1ull << 51;
    DEBUG << a << b << c <<" "<<d;
    A e;
    DEBUG << e;

    B b1;
    DEBUG << b1;
    return 0;
}