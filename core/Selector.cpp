// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Selector.h"
#include "EventLoop.h"
namespace sealedserver
{

Selector::Selector(EventLoop* loop) : m_loop(loop)
{
    m_epollfd = epoll_create(1);
	m_events  = (struct epoll_event*)malloc(MAX_NEVENTS * sizeof(struct epoll_event));    
    m_socknum = 0;

    /// make sure no problem exist in constructor
    assert(errno == 0);
}

void Selector::registerEvent(Handler *handler, short event)
{
    Socket sock = handler->getSocket();

    int fd       = sock.fd();
    int events   = event;
   	bool addFlag = true;

    if(handler->getStatus() != 0) 
    	addFlag = false;

    handler -> updateStatus(event);
    events  =  handler->getStatus();

    events |= EPOLLET;

    struct epoll_event epev = {0, {0}};
    epev.events   = events;
    epev.data.ptr = handler;

    if(addFlag == true)
    {
        assert(epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &epev) == 0);
        m_socknum++;
    }
    else
        assert(epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &epev) == 0);
}

void Selector::unRegisterEvent(Handler *handler, short event)
{
    Socket sock = handler->getSocket();
    int fd = sock.fd(), events = 0, delflag = 1;

    if(handler->getDelflag() == 1) return;

    if(event == -1)
    {
        DEBUG << "found Close Event(triggered by detach), fd: " << fd;
        if(epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL) != 0)
        {
            printf("error = %s, %d\n", strerror(errno), fd);
            assert(0);
        }
        handler->setDelflag();
        return;
    }
    else if((event & EPOLLRDHUP) || (event & EPOLLERR) || (event & EPOLLHUP))
    {
        if (event & EPOLLRDHUP)
            DEBUG << "found EPOLLRDHUP for fd: " << fd;
        else
            DEBUG << "found EPOLLERR for fd: " << fd;

        assert(epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL) == 0);
        handler->setDelflag();
        return;
    }

    handler->removeStatus(event);
    events = handler->getStatus();
    if(events != 0) delflag = 0;

    if(delflag == 0)
    {
        struct epoll_event epev = {0, {0}};
        epev.events   = events;
        epev.data.ptr = handler;
        assert(epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &epev) == 0);
    }
    else if(delflag == 1)
    {
        assert(epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL) == 0);
        m_socknum--;
    }
}

int Selector::dispatch()
{
    int num;

    int timeout = 5*1000; //5s

    num = epoll_wait(m_epollfd, m_events, MAX_NEVENTS, timeout);
    
    DEBUG << "dispatch: " << num ;
    
    for(int i = 0; i < num; i++)
    {
        int what = m_events[i].events;
        Handler *handler = (Handler*)m_events[i].data.ptr;
        m_loop->addActive(handler->getSocket().fd(), what);
    }

    m_loop->finDel();
}

};
