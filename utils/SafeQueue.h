#ifndef _SAFE_QUEUE_H
#define _SAFE_QUEUE_H

#include <queue>
using namespace std;

#include "Thread.h"
using namespace utils;

#include "stdint.h"

template<class T>
class SafeQueue 
{
  private:
    queue<T> m_queue;
    Mutex    m_mutex;
    CondVar  m_cond;

  public:
    typedef typename queue<T>::value_type value_type;

    typedef typename queue<T>::size_type size_type;

    SafeQueue() : m_cond(&m_mutex) { }

    bool empty() const 
    { 
        ScopeMutex scope(&m_mutex);
        return m_queue.empty();
    }

    size_type size() const 
    { 
        ScopeMutex scope(&m_mutex);
        return m_queue.size(); 
    }

    value_type& front() 
    { 
        ScopeMutex scope(&m_mutex); 
        return m_queue.front(); 
    }

    const value_type& front() const 
    { 
        ScopeMutex scope(&m_mutex); 
        return m_queue.front(); 
    }

    value_type& back() 
    { 
        ScopeMutex scope(&m_mutex); 
        return m_queue.back(); 
    }

    const value_type& back() const 
    { 
        ScopeMutex scope(&m_mutex); 
        return m_queue.back(); 
    }

    bool push(const value_type& v) 
    {
        ScopeMutex scope(&m_mutex);
	    m_queue.push(v);
        m_cond.signal();
        return true;
    }

    void pop() 
    { 
        ScopeMutex scope(&m_mutex);
        m_queue.pop(); 
    }

    T get() 
    {
        ScopeMutex scope(&m_mutex);
        if (empty()) 
            return T();

        T ret = front();
        pop();
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
