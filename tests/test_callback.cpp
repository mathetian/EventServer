#include "../utils/Callback.h"

#include <stdio.h>

class A
{
public:
	void a()
	{
		printf("hello, enter function a\n");
	}
};

int main()
{
	A a1;
	Callback<void> call(a1, &A::a);
	call();
	return 0;
}