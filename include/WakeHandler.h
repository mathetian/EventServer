// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _WAKE_HANDLER_H
#define _WAKE_HANDLER_H

#include "SQueue.h"
using namespace utils;

#include "MSGHandler.h"

namespace eventserver
{

class WakeHandler : public MSGHandler
{
public:
    WakeHandler(EventLoop *loop, Socket sock, SQueue<Handler*> *h_queue) : MSGHandler(loop, sock)
    {
        handler_queue_ = h_queue;
    }

public:
    void push(Handler *handler)
    {
        handler_queue_ -> push(handler);
    }

private:
    virtual void received(STATUS status, Buffer &buff)
    {
        assert(status == MSGHandler::SUCC);
        while(handler_queue_ -> empty() == false)
        {
            Handler* handler = handler_queue_ -> get();
            handler -> invoke();
        }
    }
private:
    SQueue<Handler*> *handler_queue_;
};

};

#endif