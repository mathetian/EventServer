// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _TIME_EVENT_SET_H
#define _TIME_EVENT_SET_H

#include <list>
using namespace std;

#include "Timer.h"
using namespace utils;

#include "Handler.h"

namespace sealedServer
{

struct TimeEventItem
{
    Handler *ptr;
    Timer      timer;
    TimeEventItem(Handler *ptr, Timer timer) :\
        ptr(ptr), timer(timer) { }
};

class TimeEventSet
{
    list<TimeEventItem> lists;
public:

    void insert(const TimeEventItem &item)
    {
        list<TimeEventItem>::iterator iter = lists.begin();
        for(; iter != lists.end(); iter++)
        {
            const TimeEventItem t2 = *iter;
            if(t2.timer > item.timer) break;
        }
        lists.insert(iter, item);
    }

    bool remove(const TimeEventItem &item)
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

    bool removen(int n)
    {
        list<TimeEventItem>::iterator iter = lists.begin();
        while(n--) iter++;
        lists.erase(lists.begin(), iter);

        return true;
    }

    bool exist(Handler *phandler)
    {
        list<TimeEventItem>::iterator iter = lists.begin();
        for(; iter != lists.end(); iter++)
        {
            if(phandler == (*iter).ptr)
                return true;
        }
        return false;
    }

    bool remove(Handler *phandler)
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

    bool empty()
    {
        return lists.begin() == lists.end();
    }

    const list<TimeEventItem>* getlist() const
    {
        return &lists;
    }

    class Iterator
    {
    public:
        Iterator(TimeEventSet * pset) : \
            m_pset(pset)
        {
            seekToFirst();
        }

        const TimeEventItem * next()
        {
            assert(iter != m_pset->lists.end());
            iter++;
            return &(*iter);
        }

        const TimeEventItem * prev()
        {
            assert(iter != m_pset->lists.begin());
            iter--;
            return &(*iter);
        }

        void seekToFirst()
        {
            iter = m_pset->lists.begin();
        }

        void seekToEnd()
        {
            iter = m_pset->lists.end();
        }

        const TimeEventItem * first()
        {
            return &(*(m_pset->lists.begin()));
        }

        const TimeEventItem * end()
        {
            return &(*(m_pset->lists.end()));
        }

        list<TimeEventItem>::iterator getiter()
        {
            return iter;
        }

    private:
        TimeEventSet* m_pset;
        list<TimeEventItem>::iterator iter;
    };
};

};

#endif