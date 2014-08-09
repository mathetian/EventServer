// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _MAX_HEAP_H
#define _MAX_HEAP_H

#include <queue>
using namespace std;

#include "Callback.h"

namespace utils
{

class TimerEvent
{
public:
    TimerEvent(const Callback<void> &callback, const Timer &timer)
        : callback_(callback), timer_(timer)
    {
    }

public:
    void call()
    {
        callback_();
    }

    Timer timer() const
    {
        return timer_;
    }

private:
    Callback<void> callback_;
    Timer          timer_;
    friend struct cmp;
};

struct cmp
{
    bool operator()(const TimerEvent &item_1, const TimerEvent &item_2)
    {
        if(item_1.timer_ < item_2.timer_)
            return false;
        return true;
    }
};

class TimerEvents
{
public:
    void add(const TimerEvent &item)
    {
        item_queue_.push(item);
    }

    void add(const Callback<void> &callback, const Timer &timer)
    {
        add(TimerEvent(callback, timer));
    }

    TimerEvent top() const
    {
        return item_queue_.top();
    }

    void pop()
    {
        item_queue_.pop();
    }

    bool empty()
    {
        return item_queue_.empty() == true ? true : false;
    }

private:
    priority_queue<TimerEvent, vector<TimerEvent>, cmp> item_queue_;
};

};


#endif