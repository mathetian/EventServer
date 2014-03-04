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
	int m_curid;
	typedef struct ThreadArg_t ThreadArg;
	vector<ThreadArg> thrargs;

	vector<SafeQueue<Callback<void> > > m_scv;

	Mutex m_mutex;
public:
	EventPool(int thrnum = 2) : m_thrnum(thrnum) , m_curid(0)
	{
		m_scv = vector<SafeQueue<Callback<void> > >(thrnum);

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
		/** Todo list, remove all queue, thread join**/

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
			m_scv[thrid].wait(call);
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
		
		m_curid = (m_curid + 1)%m_thrnum;
		m_scv[m_curid].push(call);
	}
};
#endif