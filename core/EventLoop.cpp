// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "EventLoop.h"
#include "EventPool.h"
#include "WakeHandler.h"

namespace eventserver
{

EventLoop::EventLoop(EventPool *pool) : m_stop(false),
    m_selector(new Selector(this)), m_pool(pool) { }

EventLoop::~EventLoop()
{
    WARN << "Destructor" ;

    map<int, Handler*>::iterator iter = m_map.begin();

    for(; iter != m_map.end(); iter++)
    {
        Handler *tmpHandler = iter->second;

        delete tmpHandler;
        tmpHandler = NULL;
    }

    m_map.clear();

    delete m_selector;
    m_selector = NULL;

    if(m_signal) delete m_signal;
    // for(int i = 0;i < m_handlers.size();i++)
    //     delete m_handlers[i].first;
}

void EventLoop::runForever()
{
    m_stop = false;

    while (!m_stop)
        runOnce();

    WARN << "Begin EventLoop Stop" ;

    map<int, Handler*>::iterator iter = m_map.begin();

    for(; iter!=m_map.end(); iter++)
    {
        Handler *handler = (*iter).second;

        DEBUG << "Stop fd: " << handler->getSocket().fd();

        m_selector->unRegisterEvent(handler, -1);

        handler->onCloseEvent(CLSSIG);
    }

    WARN << "End EventLoop Stop" ;

    m_map.clear();
}

void EventLoop::runOnce()
{
    if(m_heap.empty() == false)
    {
        TimerEvent event = m_heap.top();
        Timer cur = Timer::now();
        if(cur > event.timer())
        {
            m_selector->dispatch(0);
        }
        else
        {
            cur = event.timer() - cur;
            m_selector->dispatch(cur.to_usecs());
        }
    }
    else
    {
        m_selector->dispatch(5000);
    }

    if(m_heap.empty() == false)
    {
        Timer timer = Timer::now();
        while(m_heap.empty() == false)
        {
            TimerEvent event = m_heap.top();
            if(event.timer() > timer) break;
            m_heap.pop();
            event.call();
        }
    }

    runAllActives();
    finishDelete();
}

void EventLoop::stop()
{
    m_stop = true;
}

void EventLoop::attachHandler(int fd, Handler *p)
{
    /// won't allow attach duplicate
    assert(m_map.find(fd) == m_map.end());

    m_map[fd] = p;
}

void EventLoop::detachHandler(int fd, Handler *p)
{
    /// would be in the map
    assert(m_map.find(fd) != m_map.end() && m_map[fd] == p);
    assert(m_map.erase(fd) == 1);

    m_selector->unRegisterEvent(p, -1);
}

void EventLoop::registerRead(int fd)
{
    assert(m_map.find(fd) != m_map.end());

    m_selector->registerEvent(m_map[fd], EPOLLIN);
}

void EventLoop::registerWrite(int fd)
{
    assert(m_map.find(fd) != m_map.end());

    m_selector->registerEvent(m_map[fd], EPOLLOUT);
}

void EventLoop::unRegisterRead(int fd)
{
    assert(m_map.find(fd) != m_map.end());

    m_selector->unRegisterEvent(m_map[fd], EPOLLIN);
}

void EventLoop::unRegisterWrite(int fd)
{
    assert(m_map.find(fd) != m_map.end());

    m_selector->unRegisterEvent(m_map[fd], EPOLLOUT);
}

void EventLoop::addActive(int fd, int type)
{
    assert(m_active.find(fd) == m_active.end());

    m_active[fd] = type;
}

void EventLoop::addClosed(Handler* handler)
{
    m_del.push_back(handler);
}

void EventLoop::runAllActives()
{
    map<int, int>::iterator iter = m_active.begin();

    for(; iter != m_active.end(); iter++)
    {
        int fd = (*iter).first;
        int event = (*iter).second;

        assert(m_map.find(fd) != m_map.end());

        m_map[fd] -> proceed(event);
    }

    m_active.clear();
}

void EventLoop::finishDelete()
{
    if(m_del.size() != 0)
    {
        DEBUG << "Need to Destory " << m_del.size() \
              << " Objects" << ". Remain " << m_map.size() ;
    }

    for(int i=0; i < m_del.size(); i++)
    {
        Handler *handler = m_del[i];

        if(handler) delete handler;

        handler = NULL;
    }

    vector<Handler*> handlers;

    swap(handlers, m_del);
}

EventLoop* EventLoop::getRandomLoop(int &thrid)
{
    m_pool -> getRandomLoop(thrid);
}

void EventLoop::attach(const Callback<void> &call, const Timer &timer)
{
    m_heap.add(call, timer);
}

void EventLoop::attach(int signo, void (*sighandler)(int))
{
    m_signal -> attach(signo, sighandler);
}

void EventLoop::attach()
{
    SignalOuter &outer = SignalOuter::Instance();
    m_signal = new SignalHandler(this, outer.sockets().first);
}

void EventLoop::attach(Socket socket, int thrid)
{
    Handler *handler = new WakeHandler(this, socket);
    m_handlers[thrid] = handler;
}

};