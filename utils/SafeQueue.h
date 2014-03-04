#ifndef _SAFE_QUEUE_H
#define _SAFE_QUEUE_H

#include <queue>
using namespace std;

#include "Thread.h"
using namespace utils;

#include <stdint.h>
#include <stdio.h>
template<class T>
class SafeQueue 
{
private:
    queue<T> m_queue;
    mutable Mutex m_mutex;
    mutable CondVar       m_cond;

    typedef typename queue<T>::value_type value_type;

    typedef typename queue<T>::size_type size_type;

public:
    SafeQueue() : m_cond(&m_mutex) { }

private:
    bool empty() const 
    { 
        return m_queue.empty();
    }

    size_type size() const 
    { 
        return m_queue.size(); 
    }

    value_type& front() 
    { 
        return m_queue.front(); 
    }

    const value_type& front() const 
    { 
        return m_queue.front(); 
    }

    value_type& back() 
    { 
        return m_queue.back(); 
    }

    const value_type& back() const 
    { 
        return m_queue.back(); 
    }

    void pop() 
    { 
        m_queue.pop(); 
    }

public:
    bool push(const value_type& v) 
    {
        ScopeMutex scope(&m_mutex);
	    m_queue.push(v);
        printf("put a\n");
        m_cond.signal();
        return true;
    }

    T get() 
    {
        ScopeMutex scope(&m_mutex);

        if (empty()) 
            return T();
        
        T ret = front();
        pop();
        printf("get a\n");
        return ret;
    }

    bool wait(T& out)
    {
        m_mutex.lock();
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

    bool tryget(T& out)
    {
        ScopeMutex scope(&m_mutex);
        
        if(!empty())
        {
            out = front();
            pop();
            return true;
        }
        return false;
    }
};


#endif
