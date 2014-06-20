// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _EVENT_LOOP_H
#define _EVENT_LOOP_H

#include "Thread.h"
using namespace utils;

#include "Acceptor.h"
#include "Selector.h"

#include "MsgHandler.h"
#include "SocketHandler.h"

namespace sealedserver
{

class EventPool;

class EventLoop
{
    map<int, SocketHandler*> m_map;
    int       m_quitFlag;
    Selector *m_selector;

    vector<SocketHandler*> m_del;
    EventPool *m_pool;

    Mutex     m_mutex;

    vector<SocketHandler*>  m_needProceed;
public:
    EventLoop(EventPool *pool) : m_quitFlag(0), m_selector(new Selector(this)), m_pool(pool)
    { }

    ~EventLoop()
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

    void runforever()
    {
        m_quitFlag = 0;
        while (!m_quitFlag) run_once();

        INFO << "runforever: " << m_quitFlag ;
        WARN << "End runforever" ;
        if(m_quitFlag==2) return; // add for benchmark

        map<int,SocketHandler*>::iterator iter = m_map.begin();

        for(; iter!=m_map.end(); iter++)
        {
            SocketHandler *handler = (*iter).second;
            INFO << handler->getSocket().get_fd();
            m_selector->unRegisterEvent(handler, -1);
            handler->onCloseSocket(CLSSIG);
        }

        m_map.clear();
    }

    void run_once()
    {
        if(m_quitFlag) printf("%d\n", m_quitFlag);
        m_selector->dispatch();

        ScopeMutex scope(&m_mutex);

        for(int i=0; i<m_needProceed.size(); i++)
            m_needProceed[i]->onProceed();

        vector<SocketHandler*> tmp;
        swap(tmp, m_needProceed);
    }

    void stop(int flag=1)
    {
        m_quitFlag = flag;
        INFO << "stop" << flag << " " << m_quitFlag;
    }

    void attachHandler(int fd, SocketHandler *p)
    {
        assert(m_map.find(fd) == m_map.end());
        m_map[fd] = p;
    }

    void detachHandler(int fd, SocketHandler *p)
    {
        assert(m_map.find(fd) != m_map.end());
        assert(m_map[fd] == p);
        assert(m_map.erase(fd) == 1);
        m_selector->unRegisterEvent(p, -1);
    }

    void insert(SocketHandler *handler)
    {
        ScopeMutex scope(&m_mutex);
        m_needProceed.push_back(handler);
    }

private:
    void registerRead(int fd)
    {
        assert(m_map.find(fd) != m_map.end());
        m_selector->registerEvent(m_map[fd], EPOLLIN);
    }

    void registerWrite(int fd)
    {
        assert(m_map.find(fd) != m_map.end());
        m_selector->registerEvent(m_map[fd], EPOLLOUT);
    }

    void unRegisterRead(int fd)
    {
        assert(m_map.find(fd) != m_map.end());
        m_selector->unRegisterEvent(m_map[fd], EPOLLIN);
    }

    void unRegisterWrite(int fd)
    {
        assert(m_map.find(fd) != m_map.end());
        m_selector->unRegisterEvent(m_map[fd], EPOLLOUT);
    }

    friend class SocketHandler;

    template<class T> friend  class TCPAcceptor;

public:
    void addActive(int fd,int type)
    {
        assert(m_map.find(fd) != m_map.end());
        SocketHandler *handler = m_map[fd];
        if((type & EPOLLRDHUP) || (type & EPOLLERR) || (type & EPOLLHUP))
        {
            m_selector->unRegisterEvent(handler, -1);
            INFO << fd << " " << (type&EPOLLRDHUP) << " " << (type&EPOLLERR) << " " << (type&EPOLLHUP);

            int       error = 0;
            socklen_t errlen = sizeof(error);
            if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen) == 0)
            {
                printf("error = %s\n", strerror(error));
            }

            handler->onCloseSocket(CLSEVT);
            return;
        }
        if(handler->getdelflag() == 1) return;
        if((type & EPOLLOUT) != 0)
        {
            m_selector->unRegisterEvent(handler, EPOLLOUT);
            handler->onSendMsg();
        }
        if(handler->getdelflag() == 1) return;
        if((type & EPOLLIN) != 0)
            handler->onReceiveMsg();
    }

    void addDel(SocketHandler*handler)
    {
        m_del.push_back(handler);
    }

    void finDel()
    {
        DEBUG << "Need Destory " << m_del.size() << " Objects";

        for(int i=0; i<m_del.size(); i++)
        {
            SocketHandler *handler = m_del[i];
            if(handler) delete handler;
            handler = NULL;
        }
        vector<SocketHandler*> handlers;
        swap(handlers, m_del);
    }
};

inline void SocketHandler::attach()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->attachHandler(m_sock.get_fd(), this);
}

inline void SocketHandler::detach()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->detachHandler(m_sock.get_fd(), this);
}

inline void SocketHandler::registerRead()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->registerRead(m_sock.get_fd());
}

inline void SocketHandler::registerWrite()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->registerWrite(m_sock.get_fd());
}

inline void SocketHandler::unRegisterRead()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->unRegisterRead(m_sock.get_fd());
}

inline void SocketHandler::unRegisterWrite()
{
    assert(m_loop && m_sock.get_fd() >= 0);
    m_loop->unRegisterWrite(m_sock.get_fd());
}

inline int Selector::dispatch()
{
    int num;

    int timeout = 5*1000; //5s

    num = epoll_wait(m_epollfd, m_events, MAX_NEVENTS, timeout);
    INFO << "dispatch: " << num ;
    for(int i = 0; i < num; i++)
    {
        int what = m_events[i].events;
        SocketHandler *handler = (SocketHandler*)m_events[i].data.ptr;
        m_loop->addActive(handler->getSocket().get_fd(), what);
    }

    m_loop->finDel();
}

inline MSGHandler::MSGHandler(EventLoop* loop, Socket sock, int first) : SocketHandler(loop), first(first)
{
    m_sock = sock;
    m_loop->insert(this);
    //onProceed();
}

inline void MSGHandler::onCloseSocket(int st)
{
    if(errno != 0) DEBUG << strerror(errno);
    DEBUG << "onCloseSocket: " << st << " " << m_sock.get_fd();
    errno = 0;

    detach();
    closedSocket();
    m_sock.close();
    m_loop->addDel(this);
}

};

#endif