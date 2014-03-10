#include "../utils/SafeQueue.h"
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

class B
{
public:
    void b()
    {
        printf("hello, enter function b\n");
    }
};

int main()
{
    SafeQueue<Callback<void> * > squeue;

    A a1;
    B b1;
    Callback<void> call1(a1, &A::a);
    Callback<void> call2(b1, &B::b);
    squeue.push(&call1);
    squeue.push(&call2);

    Callback<void> *call3 = squeue.get();
    Callback<void> *call4 = squeue.get();

    (*call3)();
    (*call4)();
}