// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _RECTOR_EPOLL_H
#define _RECTOR_EPOLL_H

#include "Handler.h"

#define MAX_NEVENTS 1000000

namespace sealedserver
{

/**
** Dispatcher, use the epoll as internal dispatcher
**/
class Selector : public Noncopyable
{
public:
    /// Constructor
    ///
    /// @param loop, EventLoop
    Selector(EventLoop* loop);

public:
    /// Register Special Event of handler
    void registerEvent(Handler *handler, short event);

    /// Unregister Speical Event of handler
    void unRegisterEvent(Handler *handler, short event);

    /// Dispatch, loop will invoke this function in each round
    int dispatch();

private:
    /// m_epollfd, epoll needs this fd
    int m_epollfd;

    /// m_scknum, counter of socket number
    /// decrease enumeration of sockets
    int m_socknum;

    /// m_loop, EventLoop
    EventLoop          *m_loop;
    
    /// m_events, each time write in it
    struct epoll_event *m_events;
};

};

#endif