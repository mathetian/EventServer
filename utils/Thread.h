#ifndef _THREAD_WIN_POS_H
#define _THREAD_WIN_POS_H

#ifdef __WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <assert.h>
#include <stdio.h>

namespace utils
{

#ifdef __WIN32
typedef HANDLE id_type;
typedef unsigned(WINAPI *Task)(void *);
#define THRINIT NULL
#else
typedef pthread_t id_type;
typedef void *(*Task)(void *);
#define THRINIT -1
#endif

class Thread
{
public:
    Thread(Task task = NULL, void * args = NULL) : m_task(task), m_args(args), m_tid(THRINIT) { }
    Thread(const Thread & thr) : m_task(thr.m_task), m_args(thr.m_args), m_tid(thr.m_tid) { }
    id_type  run()
    {
        if(m_tid != -1)
            return m_tid;

        pthread_create(&m_tid, NULL, m_task, m_args);
        return m_tid;
    }

    id_type reRun()
    {
        pthread_create(&m_tid, NULL, m_task, m_args);
        
        return m_tid;
    }
    
    void     join()
    {
        if(m_tid != -1)
            pthread_join(m_tid, NULL);
    }

    void     cancel()
    {
        if(m_tid != -1)
            pthread_cancel(m_tid);
    }

    static id_type  getIDType()
    {
        return pthread_self();
    }

private:
    id_type  m_tid;
    id_type  aaa;
    Task   m_task;
    void * m_args;
};

class CondVar;

class Mutex
{
public:
    Mutex()
    {
        pthread_mutex_init(&m_mutex, 0);
    }
    virtual ~Mutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }

public:
    void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }

    int trylock()
    {
        return pthread_mutex_trylock(&m_mutex);
    }


private:
#ifdef __WIN32
    CRITICAL_SECTION m_mutex;
#else
    pthread_mutex_t m_mutex;
#endif
    friend class CondVar;
};

class ReentrantLock;

class CondVar
{
public:
    CondVar(Mutex* mutex) : m_mutex(mutex), m_lock(NULL)
    {
        pthread_cond_init(&m_cond, NULL);
    }

    CondVar(ReentrantLock * lock) : m_mutex(NULL), m_lock(lock)
    {
        pthread_cond_init(&m_cond, NULL);
    }

    ~CondVar()
    {
        pthread_cond_destroy(&m_cond);
    }

    void wait();

    void signal()
    {
        pthread_cond_signal(&m_cond);
    }

    void signalAll()
    {
        pthread_cond_broadcast(&m_cond);
    }

private:
#ifdef __WIN32
    HANDLE m_cond;
#else
    pthread_cond_t m_cond;
#endif
    Mutex * m_mutex;
    ReentrantLock * m_lock;
};

class Noncopyable
{
protected:
    Noncopyable() { }
    ~Noncopyable() { }

private:
    Noncopyable( const Noncopyable& );
    Noncopyable& operator=( const Noncopyable& );
};

class SingletonMutex : Noncopyable
{
public:
    static SingletonMutex& getInstance()
    {
        static SingletonMutex instance;
        return instance;
    }

private:
    SingletonMutex() {}

public:
    Mutex m;
};

class ScopeMutex : Noncopyable
{
public:
    ScopeMutex(Mutex * pmutex);
    ScopeMutex(ReentrantLock * lock);
    ~ScopeMutex();

private:
    Mutex * m_pmutex;
    ReentrantLock * m_lock;
};

/**constructor can't be empty parameter.**/
class RWLock
{
public:
    RWLock(Mutex * mutex = NULL) : m_mutex(mutex), m_condRead(m_mutex),
        m_condWrite(m_mutex), m_nReader(0), m_nWriter(0), m_wReader(0), m_wWriter(0)
    {
        if(mutex == NULL) m_mutex = new Mutex;
    }

    void readLock()
    {
        ScopeMutex scope(m_mutex);
        if(m_nWriter || m_wWriter)
        {
            m_wReader++;
            while(m_nWriter || m_wWriter)
                m_condRead.wait();
            m_wReader--;
        }
        m_nReader++;
    }

    void readUnlock()
    {
        ScopeMutex scope(m_mutex);
        m_nReader--;

        if(m_wWriter != 0)
            m_condWrite.signal();
        else if(m_wReader != 0)
            m_condRead.signal();
    }

    void writeLock()
    {
        ScopeMutex scope(m_mutex);
        if(m_nReader || m_nWriter)
        {
            m_wWriter++;
            while(m_nReader || m_nWriter)
                m_condWrite.wait();
            m_wWriter--;
        }
        m_nWriter++;
    }

    void writeUnlock()
    {
        ScopeMutex scope(m_mutex);
        m_nWriter--;

        if(m_wWriter != 0)
            m_condWrite.signal();
        else if(m_wReader != 0)
            m_condRead.signal();
    }


private:
    int  m_nReader;
    int  m_nWriter;
    int  m_wReader;
    int  m_wWriter;

private:
    Mutex * m_mutex;
    CondVar m_condRead;
    CondVar m_condWrite;
};

/**Todo list**/
class ReentrantLock
{
public:
    ReentrantLock() : m_id(-1), m_cond(&m_tmplock), m_time(0) { }

    void lock()
    {
        m_tmplock.lock();

        if(m_id == -1)
        {
            assert(m_lock.trylock() == 0);
            m_id = pthread_self();
            m_tmplock.unlock();
            m_time = 1;
            return;
        }
        else if(m_id == pthread_self())
        {
            m_tmplock.unlock();
            m_time++;
            return;
        }

        while(m_id != -1) m_cond.wait();
        m_id = pthread_self();
        m_time = 1;
        m_tmplock.unlock();
    }

    void unlock()
    {
        m_tmplock.lock();

        m_time--;
        if(m_time == 0)
        {
            m_lock.unlock();
            m_id = -1;
            m_tmplock.unlock();
            m_cond.signal();
        }
        else
            m_tmplock.unlock();
    }

    bool trylock()
    {
        m_tmplock.lock();
        if(m_id != -1)
        {
            m_tmplock.unlock();
            return false;
        }
        else
        {
            assert(m_lock.trylock() == 0);
            m_id = pthread_self();
            m_tmplock.unlock();
            m_time = 1;
            return true;
        }
    }

private:
    Mutex m_lock;
    id_type m_id;
    Mutex m_tmplock;
    CondVar m_cond;
    int   m_time;

    friend class CondVar;
    friend class ScopeMutex;
};

inline void CondVar::wait()
{
    if(m_mutex)
        pthread_cond_wait(&m_cond, &m_mutex->m_mutex);
    else
        pthread_cond_wait(&m_cond, &m_lock->m_lock.m_mutex);
}

inline ScopeMutex::ScopeMutex(Mutex * pmutex) :\
    m_pmutex(pmutex), m_lock(NULL)
{
    m_pmutex -> lock();
}

inline ScopeMutex::ScopeMutex(ReentrantLock *lock):\
    m_pmutex(NULL), m_lock(lock)
{
    m_lock -> lock();
}

inline ScopeMutex::~ScopeMutex()
{
    m_pmutex -> unlock();
}

};

#endif