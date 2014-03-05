#ifndef _EVENT_POOL_H
#define _EVENT_POOL_H

#include "../utils/Thread.h"
using namespace utils;

#include "../utils/Log.h"
#include "../utils/SafeQueue.h"
#include "../utils/Callback.h"

#include <vector>
using namespace std;

class EventPool
{
	struct ThreadArg_t
	{
		EventPool *ep;
		void* (EventPool::*func)(int);
		int id;
	};

	int m_thrnum;
	vector<Thread*> threads;
	typedef struct ThreadArg_t ThreadArg;
	vector<ThreadArg> thrargs;

	SafeQueue<Callback<void> > m_squeue;
	Mutex m_mutex;
public:
	EventPool(int thrnum = 2) : m_thrnum(thrnum)
	{
		threads = vector<Thread*>(thrnum);
		thrargs   = vector<ThreadArg>(thrnum);

		for(int i=0;i<thrnum;i++)
		{
			thrargs[i].ep   = this;
			thrargs[i].func = &EventPool::ThreadBody;
			thrargs[i].id   = i;
			threads[i] = new Thread(ThreadFunc, &(thrargs[i]));
		}

		for(int i=0;i<thrnum;i++) threads[i]->run();
	}
	
	~EventPool()
	{
		/** Todo list, remove queue, thread join**/

		for(int i=0;i<m_thrnum;i++)
		{
			threads[i]->join();
			delete threads[i];
		}
	}

	void* ThreadBody(int thrid)
	{
		INFO << "Enter Thread: " << thrid; 
		Callback<void> call;
		while(true)
		{
			m_squeue.wait(call);
			call();
		}
	}

	static void* ThreadFunc(void*arg)
	{
		ThreadArg targ = *(ThreadArg*)arg;
		((targ.ep)->*(targ.func))(targ.id);
	}

	void insert(Callback<void> call)
	{
		ScopeMutex scope(&m_mutex);
		m_squeue.push(call);
	}
};
#endif