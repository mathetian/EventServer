// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "TimeEvents.h"

namespace eventserver
{

struct TimeEventSet::TimeEventItem
{
    Handler *ptr;
    Timer      timer;
    TimeEventItem(Handler *ptr, Timer timer) :\
        ptr(ptr), timer(timer) { }
};

void TimeEventSet::insert(const TimeEventItem &item)
{
    list<TimeEventItem>::iterator iter = lists.begin();
    for(; iter != lists.end(); iter++)
    {
        const TimeEventItem t2 = *iter;
        if(t2.timer > item.timer) break;
    }
    lists.insert(iter, item);
}

bool TimeEventSet::remove(const TimeEventItem &item)
{
    list<TimeEventItem>::iterator iter = lists.begin();
    for(; iter != lists.end(); iter++)
    {
        const TimeEventItem t2 = *iter;
        if(t2.timer > item.timer) break;
        else if(t2.timer == item.timer && item.ptr == t2.ptr)
            break;
    }
    if(iter != lists.end() && (*iter).timer == item.timer)
    {
        lists.erase(iter);
        return true;
    }
    return false;
}

bool TimeEventSet::removen(int n)
{
    list<TimeEventItem>::iterator iter = lists.begin();
    while(n--) iter++;
    lists.erase(lists.begin(), iter);

    return true;
}

bool TimeEventSet::exist(Handler *phandler)
{
    list<TimeEventItem>::iterator iter = lists.begin();
    for(; iter != lists.end(); iter++)
    {
        if(phandler == (*iter).ptr)
            return true;
    }
    return false;
}

bool TimeEventSet::remove(Handler *phandler)
{
    list<TimeEventItem>::iterator iter = lists.begin();
    for(; iter != lists.end(); iter++)
    {
        if(phandler == (*iter).ptr)
        {
            lists.erase(iter);
            return true;
        }
    }
    return false;
}

bool TimeEventSet::empty()
{
    return lists.begin() == lists.end();
}

const list<TimeEventSet::TimeEventItem>* TimeEventSet::getlist() const
{
    return &lists;
}


TimeEventSet::Iterator::Iterator(TimeEventSet * pset) : m_pset(pset)
{
    seekToFirst();
}

const TimeEventSet::TimeEventItem * TimeEventSet::Iterator::next()
{
    assert(iter != m_pset->lists.end());
    iter++;
    return &(*iter);
}

const TimeEventSet::TimeEventItem * TimeEventSet::Iterator::prev()
{
    assert(iter != m_pset->lists.begin());
    iter--;
    return &(*iter);
}

void TimeEventSet::Iterator::seekToFirst()
{
    iter = m_pset->lists.begin();
}

void TimeEventSet::Iterator::seekToEnd()
{
    iter = m_pset->lists.end();
}

const TimeEventSet::TimeEventItem * TimeEventSet::Iterator::first()
{
    return &(*(m_pset->lists.begin()));
}

const TimeEventSet::TimeEventItem * TimeEventSet::Iterator::end()
{
    return &(*(m_pset->lists.end()));
}

list<TimeEventSet::TimeEventItem>::iterator TimeEventSet::Iterator::getiter()
{
    return iter;
}

};