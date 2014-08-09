// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Selector.h"
#include "EventLoop.h"

namespace eventserver
{

Selector::Selector(EventLoop* loop) : m_loop(loop), m_socknum(0)
{
    m_epollfd = epoll_create(1);
    m_events  = (struct epoll_event*)malloc(MAX_NEVENTS * sizeof(struct epoll_event));

    /// make sure no problem exists in constructor
    assert(errno == 0);
}

Selector::~Selector()
{
    ::close(m_epollfd);
    free(m_events);
}

void Selector::registerEvent(Handler *handler, short event)
{
    Socket sock  = handler -> getSocket();

    int  fd      = sock.fd();
    int  events  = event;
    bool addFlag = true;

    /// Firstly, check the status
    /// Then determining whether `add` or `modify`

    if(handler -> getStatus() != 0)
        addFlag = false;

    handler -> updateStatus(event);
    events  =  handler -> getStatus();

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
    Socket sock = handler -> getSocket();
    int fd       = sock.fd();
    int events   = 0;
    bool delflag = true;

    /// Has been put into remove list
    if(handler->getDelflag() == true) return;

    /// if event == -1, delete it from selector
    /// else if event or EPOLLRDHUP or  EPOLLERR or  EPOLLHUP, delete it from selector
    /// else
    if(event == -1)
    {
        DEBUG << "Found Close Event(triggered by detach), fd: " << fd;

        assert(epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL) == 0);
        handler -> setDelflag();
    }
    else if((event & EPOLLRDHUP) || (event & EPOLLERR) || (event & EPOLLHUP))
    {
        if (event & EPOLLRDHUP)
            DEBUG << "Found EPOLLRDHUP for fd: " << fd;
        else if(event & EPOLLERR)
            DEBUG << "Found EPOLLERR for fd: " << fd;
        else
            DEBUG << "Found EPOLLHUP for fd: " << fd;

        assert(epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL) == 0);
        handler -> setDelflag();
    }
    else
    {
        /// Firstly, remove event from event-list
        /// Then   , get the updated status

        handler -> removeStatus(event);
        events = handler -> getStatus();

        if(events != 0) delflag = false;

        if(delflag == false)
        {
            struct epoll_event epev = {0, {0}};
            epev.events   = events;
            epev.data.ptr = handler;
            assert(epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &epev) == 0);
        }
        else
        {
            assert(epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL) == 0);
            m_socknum--;
        }
    }
}

int Selector::dispatch(int millisecond)
{
    int num = epoll_wait(m_epollfd, m_events, MAX_NEVENTS, millisecond);

    if(num != 0) DEBUG << "Dispatch, numbef of events: " << num ;

    for(int i = 0; i < num; i++)
    {
        int what = m_events[i].events;
        Handler *handler = (Handler*)m_events[i].data.ptr;
        m_loop->addActive(handler -> getSocket().fd(), what);
    }
}

};
