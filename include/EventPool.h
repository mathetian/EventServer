#ifndef _EVENT_POOL_H
#define _EVENT_POOL_H

#include <vector>
using namespace std;

#include "../utils/Log.h"
#include "../utils/Thread.h"
#include "../utils/SafeQueue.h"
#include "../utils/Callback.h"
using namespace utils;

#include "EventLoop.h"
#include "Acceptor.h"

class EventPool
{
    struct ThreadArg_t
    {
        EventPool *ep;
        void* (EventPool::*func)(int);
        int id;
    };

    vector<Thread*> threads;
    typedef struct ThreadArg_t ThreadArg;
    vector<ThreadArg> thrargs;

private:
    vector<EventLoop*> loops;
    int m_thrnum;

public:
    EventPool(int thrnum = 2) : m_thrnum(thrnum)
    {
        threads  = vector<Thread*>(m_thrnum);
        thrargs  = vector<ThreadArg>(m_thrnum);

        for(int i=0; i<m_thrnum; i++)
        {
            thrargs[i].ep   = this;
            thrargs[i].func = &EventPool::ThreadBody;
            thrargs[i].id   = i;
            threads[i] = new Thread(ThreadFunc, &(thrargs[i]));
        }

        loops = vector<EventLoop*>(m_thrnum, NULL);
        for(int i=0; i<m_thrnum ; i++)
            loops[i] = new EventLoop(this);

        for(int i=0; i<m_thrnum; i++) threads[i]->run();
    }

    void* ThreadBody(int thrID)
    {
        INFO << "Thread " << thrID << " Running";
        loops[thrID]->runforever();
    }

    static void* ThreadFunc(void *arg)
    {
        ThreadArg targ = *(ThreadArg*)arg;
        ((targ.ep)->*(targ.func))(targ.id);
    }

    EventLoop* getRandomLoop()
    {
        INFO << "getRandomLoop: " << m_thrnum << ", loops: " << loops.size();
        return loops.at(rand()%m_thrnum);
    }

    void  runforever()
    {
        for(int i=0; i<m_thrnum;i++)
            threads[i]->join();
    }

    int getNum() const 
    {
        return m_thrnum;
    }

    EventLoop* getLoop(int id)
    {
        return loops.at(id);
    }
};

inline EventLoop* SocketHandler::getLoop2()
{
    int id = rand()%(m_loop->m_pool->getNum());
    return m_loop->m_pool->getLoop(id);
}

#endif