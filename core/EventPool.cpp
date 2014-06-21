// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "EventPool.h"

namespace sealedserver
{

struct EventPool::ThreadArg_t
{
    EventPool *ep;
    void* (EventPool::*func)(int);
    int id;
};

EventPool::EventPool(int loopNum) : m_loopNum(loopNum)
{
	m_threads = new Thread*   [loopNum];
	m_args    = new ThreadArg [loopNum];
	m_loops   = new EventLoop*[loopNum];

    for(int i=0; i < loopNum; i++)
    {
    	m_args[i].ep   = this;
        m_args[i].func = &EventPool::ThreadBody;
        m_args[i].id   = i;

    	m_threads[i] = new Thread(ThreadFunc, &(m_args[i]));
    }

    for(int i=0; i < loopNum ; i++)
        m_loops[i] = new EventLoop(this);
}

EventPool::~EventPool()
{
	for(int i = 0;i < m_loopNum;i++)
	{
		delete m_threads[i];
		m_threads[i] = NULL;
	}

	delete [] m_threads;

	m_threads = NULL;

	delete [] m_args;
	//delete [] m_loops;

	m_args = NULL; //m_loops = NULL;
}

void EventPool::run()
{
	for(int i=0; i < m_loopNum; i++)
        m_threads[i]->run();

    for(int i=0; i<m_loopNum; i++)
        m_threads[i]->join();
}

void EventPool::stop()
{
    for(int i=0;i < m_loopNum; i++)
        m_loops[i] -> stop();

    WARN << "End closeAllLoop" ;
}

EventLoop* EventPool::getRandomLoop()
{
    return m_loops[rand()%m_loopNum];
}

int        EventPool::getLoopNum() const
{
    return m_loopNum;
}

EventLoop* EventPool::getLoopByID(int id)
{
	assert(id >= 0 && id < m_loopNum);

    return m_loops[id];
}

void* EventPool::ThreadBody(int id)
{
    INFO << "Loop " << id << " Running";

    m_loops[id]->runForever();
    
    WARN << "Loop " << id << " Finished";
}

void* EventPool::ThreadFunc(void *arg)
{
    ThreadArg targ = *(ThreadArg*)arg;

    ((targ.ep)->*(targ.func))(targ.id);
        
    pthread_exit(NULL);
}

};