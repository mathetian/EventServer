// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "EventLoop.h"

namespace sealedserver
{

EventLoop::EventLoop(EventPool *pool) : m_stop(false), 
	m_selector(new Selector(this)), m_pool(pool) { }

EventLoop::~EventLoop()
{
    map<int, SocketHandler*>::iterator iter = m_map.begin();
    
    for(; iter != m_map.end(); iter++)
    {
        SocketHandler *tmpHandler = iter->second;
        
        delete tmpHandler;
        tmpHandler = NULL;
    }

    m_map.clear();
    
    delete m_selector;
    m_selector = NULL;
}

void EventLoop::runForever()
{
    m_stop = false;

    while (!m_stop) 
        runOnce();

    WARN << "EventLoop Stopped" ;
   
    map<int, SocketHandler*>::iterator iter = m_map.begin();

    for(; iter!=m_map.end(); iter++)
    {
        SocketHandler *handler = (*iter).second;

        INFO << handler->getSocket().get_fd();
        
        m_selector->unRegisterEvent(handler, -1);
        
        handler->onCloseSocket(CLSSIG);
    }

    m_map.clear();
}

void EventLoop::runOnce()
{   
    m_selector->dispatch();

    ScopeMutex scope(&m_mutex);

    for(int i=0; i < m_active.size(); i++)
        m_active[i]->onProceed();

    vector<SocketHandler*> tmp;
    swap(tmp, m_active);
}

void EventLoop::stop()
{
    m_stop = true;
}

/// won't allow attach duplicate
void EventLoop::attachHandler(int fd, SocketHandler *p)
{
    assert(m_map.find(fd) == m_map.end());
    
    m_map[fd] = p;
}

void EventLoop::detachHandler(int fd, SocketHandler *p)
{
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
    assert(m_map.find(fd) != m_map.end());

    SocketHandler *handler = m_map[fd];
    
    if((type & EPOLLRDHUP) || (type & EPOLLERR) || (type & EPOLLHUP))
    {
        m_selector->unRegisterEvent(handler, -1);

        INFO << fd << " " << (type&EPOLLRDHUP) << " " << (type&EPOLLERR) << " " << (type&EPOLLHUP);

        /**
        ** get the information for errno
        **/
        int       error = 0;
        socklen_t errlen = sizeof(error);
        
        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen) == 0)
        {
            INFO <<  "error = " << strerror(error));
        }

        handler->onCloseSocket(CLSEVT);

        return;
    }
    
    if(handler -> getdelflag() == 1) 
        return;
    
    if((type & EPOLLOUT) != 0)
    {
        m_selector->unRegisterEvent(handler, EPOLLOUT);
        handler->onSendMsg();
    }
    
    if(handler->getdelflag() == 1) 
        return;
    
    if((type & EPOLLIN) != 0)
        handler -> onReceiveMsg();
}

void EventLoop::addDel(SocketHandler* handler)
{
    m_del.push_back(handler);
}

void EventLoop::finDel()
{
    INFO << "Need Destory " << m_del.size() << " Objects";

    for(int i=0; i < m_del.size(); i++)
    {
        SocketHandler *handler = m_del[i];
        
        if(handler) 
            delete handler;
        
        handler = NULL;
    }

    vector<SocketHandler*> handlers;
    
    swap(handlers, m_del);
}

};