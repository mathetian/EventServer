// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _SAFE_QUEUE_H
#define _SAFE_QUEUE_H

#include "Thread.h"

/**
** A threadsafe queue class
**/

namespace utils
{

template<class T>
class SQueue : public Noncopyable
{
private:
    queue<T> m_queue;
    mutable ReentrantLock  m_mutex;
    mutable CondVar        m_cond;

    /// The type of datum stored in the queue.
    typedef typename queue<T>::value_type value_type;
    /// The type used to represent the size of the queue.
    typedef typename queue<T>::size_type size_type;

public:
    /// Constructor.
    SQueue() : m_cond(&m_mutex) { }

public:
    /// Returns true if the queue is empty.
    bool empty() const
    {
        ScopeMutex scope(&m_mutex);

        return m_queue.empty();
    }

    /// Returns the number of items in the queue.
    size_type size() const
    {
        ScopeMutex scope(&m_mutex);

        return m_queue.size();
    }

    /// Returns the item at the head of the queue (least recently pushed).
    value_type& front()
    {
        ScopeMutex scope(&m_mutex);

        return m_queue.front();
    }

    /// Returns the item at the head of the queue (least recently pushed).
    const value_type& front() const
    {
        ScopeMutex scope(&m_mutex);

        return m_queue.front();
    }

    /// Returns the item at the tail of the queue (most recently pushed).
    value_type& back()
    {
        ScopeMutex scope(&m_mutex);
        return m_queue.back();
    }

    /// Returns the item at the tail of the queue (most recently pushed).
    const value_type& back() const
    {
        ScopeMutex scope(&m_mutex);
        return m_queue.back();
    }

    /// Adds an item to the back of the queue.  Returns true if the item
    /// will be immediately dispatched.
    bool push(const value_type& v)
    {
        ScopeMutex scope(&m_mutex);

        m_queue.push(v);
        m_cond.signal();

        return true;
    }

    /// Removes an item from the front of the queue.
    void pop()
    {
        ScopeMutex scope(&m_mutex);
        m_queue.pop();
    }

    /// Returns and removes an item from the front of the queue.
    /// If not suitable item, return NULL
    T get()
    {
        ScopeMutex scope(&m_mutex);

        if (empty())
            return T();

        T ret = front();
        pop();

        return ret;
    }

    /// Returns and removes an item from the front of the queue.
    /// Wait utils valid item is found
    bool wait(T& out)
    {
        ScopeMutex scope(&m_mutex);

        if(!empty())
        {
            out = front();
            pop();
            m_mutex.unlock();
            return true;
        }

        while(empty())
            m_cond.wait();

        out = front();
        pop();
        m_mutex.unlock();

        return true;
    }
};

};


#endif
