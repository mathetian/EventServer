#include "../utils/Log.h"
#include "../utils/Thread.h"
using namespace utils;

#include <vector>
using namespace std;

#include <unistd.h>

// class B
// {

// };

// class A
// {
// 	struct ThreadArg_t
//     {
//         A *ep;
//         void* (A::*func)(int);
//         int id;
//     };

//     vector<Thread*> threads;
//     typedef struct ThreadArg_t ThreadArg;
//     vector<ThreadArg> thrargs;

// private:
//     vector<EventLoop*> loops;
//     int m_thrnum;

// public:
// 	A(int thrnum=3) : m_thrnum(thrnum)
// 	{
// 		 threads  = vector<Thread*>(m_thrnum);
//         thrargs  = vector<ThreadArg>(m_thrnum);

//         for(int i=0; i<m_thrnum; i++)
//         {
//             thrargs[i].ep   = this;
//             thrargs[i].func = &EventPool::ThreadBody;
//             thrargs[i].id   = i;
//             threads[i] = new Thread(ThreadFunc, &(thrargs[i]));
//         }

//         loops = vector<B*>(m_thrnum, NULL);
//         for(int i=0; i<m_thrnum ; i++)
//             loops[i] = new B(this);

//         reRun();
// 	}
// };

void* Func1(void*arg)
{
    INFO << "Begin Func1" ;
    usleep(10*100000);
    INFO << "End   Func1";
}

void RunTest()
{
    INFO << "Begin of RunTest" ;
    Thread thread(Func1,NULL);
    thread.run();

    thread.join();
    INFO << "End of RunTest";
}

int main()
{
    RunTest();
    return 0;
}