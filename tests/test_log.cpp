#include "../utils/Log.h"

#include <stdint.h>

int main()
{
	uint32_t a = 3;
	DEBUG << a;
	INFO << 31 << " " << 31;
	return 0;
}