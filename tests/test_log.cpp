#include <stdint.h>

#include "Log.h"
using namespace utils;

int main()
{
    uint32_t a = 3;
    DEBUG << a;
    INFO << 31 << " " << 31;
    return 0;
}