// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _EVENT_POOL_H
#define _EVENT_POOL_H

#include "Log.h"
#include "Thread.h"
#include "SQueue.h"
#include "Callback.h"
#include "Noncopyable.h"
using namespace utils;

#include "EventLoop.h"
#include "Acceptor.h"

namespace sealedserver
{

/**
** The pool of loops
**/
class EventPool : public Noncopyable
{
public:
    /// Constructor, the default number of loops would be 2
    EventPool(int loopNum = 2);

    /// Destructor, deallocate heap
    virtual ~EventPool();

public:
    /// start the loop and wait in here
    void       run();

    /// stop(or kill) all loops
    void       stop();

    /// get RandomLoop(), to find a suitable loop
    EventLoop* getRandomLoop();

    /// get Loop by Index(or ID)
    EventLoop* getLoopByID(int id);

    /// get the Number of Loops
    int        getLoopNum() const;

private:
    /// Threadxxxx is used to assist the `run`
    void*        ThreadBody(int id);
    static void* ThreadFunc(void *arg);

private:
    struct ThreadArg_t;
    typedef struct ThreadArg_t ThreadArg;

private:
    Thread           **m_threads;
    ThreadArg        * m_args;
    EventLoop        **m_loops;
    int                m_loopNum;
};



};

#endif