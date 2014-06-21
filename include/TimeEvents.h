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

class TimeEventSet : public Noncopyable
{
public:
    /// Constructor
    TimeEventSet() { }

    /// Destructor
    ~TimeEventSet() { }

public:
    /// Add item to Set, Sort by the time
    void insert(const TimeEventItem &item);

    /// Remove recently-item from set
    bool remove(const TimeEventItem &item);

    /// Remove consective n items(recently)
    bool removen(int n);

    /// Find whether exist in set
    ///
    /// @param pHandler, need to be found
    /// @return true, exist
    ///        false, not exist
    bool exist(Handler *pHandler);

    /// Remove item by handelr
    ///
    /// @param pHandler, need to be removed
    /// @return true, successful
    ///        false, failed(not exist)
    bool remove(Handler *pHandler)
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

    /// Return the status of the set
    bool empty()
    {
        return lists.begin() == lists.end();
    }

    /// Return the set by list
    const list<TimeEventItem>* getlist() const
    {
        return &lists;
    }

    /// Internal Iterator
    class Iterator
    {
    public:
        /// Constructor
        Iterator(TimeEventSet * pset);

        /// Step to next
        const TimeEventItem * next();

        /// Step to prev
        const TimeEventItem * prev();

        /// Step to the first element
        void seekToFirst();

        /// Step to the last element
        void seekToEnd();

        /// Return the first elemtn
        const TimeEventItem * first();

        /// Return the last element
        const TimeEventItem * end();

        /// Get the iterator
        list<TimeEventItem>::iterator getiter();

    private:
        TimeEventSet* m_pset;
        list<TimeEventItem>::iterator iter;
    };

private:
    struct TimeEventItem;
    list<TimeEventItem> lists;
};

};

#endif