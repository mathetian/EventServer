#ifndef _EVENT_LOOP_H
#define _EVENT_LOOP_H

#include <map>
using namespace std;

#include "SocketHandler.h"
#include "TimeEventSet.h"
#include "EventPool.h"
#include "TCPAcceptor.h"

#include "../utils/Log.h"
#include "../utils/Callback.h"

#include "../utils/Thread.h"
using namespace::utils;

class EventLoop
{
    EventLoop(const EventLoop &);
    EventLoop& operator = (const EventLoop &);

    map<int, SocketHandler*> m_map;

    fd_set m_readfds;
    int    m_maxfd;
    bool   m_quitFlag;
    int    m_totalSelect;

    TimeEventSet timeSets;
    Mutex  m_lock;

public:
    EventPool m_pool;

    int    m_listenSocket;
public:
    EventLoop(int thrnum = 2) : m_quitFlag(false), m_pool(thrnum), m_listenSocket(-1)
    {
        FD_ZERO(&m_readfds);
        m_maxfd       = -1;
        m_totalSelect =  0;
    }

    ~EventLoop()
    {
        map<int, SocketHandler*>::iterator iter = m_map.begin();
        for(; iter != m_map.end(); iter++)
        {
            SocketHandler *tmpHandler = iter->second;
            delete tmpHandler;
            tmpHandler = NULL;
        }
        m_map.clear();
    }

    void runforever()
    {
        while (!m_quitFlag)
            run_once();
    }

    void run_once()
    {
        if (m_quitFlag)
            return;

        fd_set rr;
        int m_mfd, m_tsel;

        DEBUG << "Begin TimeStamp";
        TimeStamp next = fireTimer();

        {
            ScopeMutex scope(&m_lock);

            rr     = m_readfds;

            m_mfd  = m_maxfd;
            m_tsel = m_totalSelect;
        }

        DEBUG << "Begin Select, maxfd: " << m_mfd << "(total: " << m_tsel << ")";
        int num;

        if (next)
        {
            struct timeval tv = next.to_timeval();
            DEBUG << "Calling ::select(); waiting " << next.to_msecs() << "ms";
            num = ::select(m_mfd+1, &rr, 0, 0, &tv);
        }
        else
        {
            DEBUG << "Calling ::select() (1 seconds)";
            next = TimeStamp(1000000);
            struct timeval tv = next.to_timeval();
            num = ::select(m_mfd+1, &rr, 0, 0, &tv);
        }

        INFO << "Need SELECTED: "<< num;
        ScopeMutex scope(&m_lock);

        int curnum = 0;
        for (int i = 0; i <= m_mfd && curnum < num; ++i)
        {
            if (FD_ISSET(i, &rr))
            {
                curnum++; assert(m_map.find(i) != m_map.end());
                Callback<void> call(*m_map[i], &SocketHandler::onReceiveMsg);
                DEBUG << "INSERT: " << i ;
                if(i != m_listenSocket)
                {
                    m_totalSelect--;
                    FD_CLR(i, &m_readfds);
                }
                m_pool.insert(call);
            }
        }

        for(int i = m_mfd; i >= 0; i--)
        {
            if(i == m_listenSocket || !FD_ISSET(i, &rr)) continue;
        }
    }

    void attachHandler(int fd, SocketHandler *p)
    {
        ScopeMutex scope(&m_lock);

        assert(fd >= 0 && fd < FD_SETSIZE);
        assert(m_map.find(fd) == m_map.end());

        m_map[fd] = p;
    }

    void detachHandler(int fd, SocketHandler *p)
    {
        ScopeMutex scope(&m_lock);

        assert(fd >= 0 && fd < FD_SETSIZE);
        assert(m_map.find(fd) != m_map.end());
        assert(m_map[fd] == p);

        // assert(m_map.erase(fd) == 1);

        if(m_maxfd == fd) m_maxfd--;

        if(FD_ISSET(fd, &m_readfds)) m_totalSelect--;

        FD_CLR(fd, &m_readfds);

        p->closeSocketForLoop();
        //  delete p;
    }

private:
    void waitTimer(int fd, uint64_t second)
    {
        ScopeMutex scope(&m_lock);

        assert(fd >= 0 && fd < FD_SETSIZE);
        assert(m_map.find(fd) != m_map.end());

        TimeStamp tms(second*1000000);
        SocketHandler *handler = m_map[fd];
        timeSets.insert(TimeEventItem(handler, TimeStamp::now() + tms));
    }

    void waitTimer(int fd, const TimeStamp &tms)
    {
        ScopeMutex scope(&m_lock);

        assert(fd >= 0 && fd < FD_SETSIZE);
        assert(m_map.find(fd) != m_map.end());

        SocketHandler *handler = m_map[fd];
        timeSets.insert(TimeEventItem(handler, TimeStamp::now() + tms));
    }

    void registerRead(int fd)
    {
        ScopeMutex scope(&m_lock);

        assert(fd >= 0 && fd < FD_SETSIZE);
        assert(m_map.find(fd) != m_map.end());

        m_maxfd   = max(m_maxfd, fd);
        FD_SET(fd, &m_readfds);
        m_totalSelect++;
    }

    TimeStamp fireTimer()
    {
        ScopeMutex scope(&m_lock);

        int count = 0;
        vector<const TimeEventItem*> fires;

        TimeStamp start = TimeStamp::now();
        TimeEventSet::Iterator iter(&timeSets);

        for(const TimeEventItem *node = iter.first(); node != iter.end(); node = iter.next())
        {
            if((node->timer).returnv() <= start.returnv())
            {
                fires.push_back(node);
                ++count;
            }
            else break;
        }

        DEBUG << count << " Timer Event would be fired";

        Callback<void> call;
        for(int i=0; i<count; i++)
        {
            call = Callback<void>(fires[i]->ptr,&SocketHandler::TimerEvent);
            m_pool.insert(call);
        }

        if(count != 0)
            timeSets.removen(count);

        if (timeSets.empty())
        {
            return TimeStamp::none();
        }
        else
        {
            TimeStamp tms = iter.first()->timer;

            TimeStamp tms2 = tms - TimeStamp::now();

            if (tms2 > TimeStamp::usecs(0LL))
                return tms2;
            else
                return TimeStamp::usecs(0LL);
        }

    }

    bool existTimer(int fd)
    {
        assert(fd >= 0 && fd < FD_SETSIZE);
        assert(m_map.find(fd) != m_map.end());

        return timeSets.exist(m_map[fd]);
    }

    void deleteTimer(int fd)
    {
        assert(fd >= 0 && fd < FD_SETSIZE);
        assert(m_map.find(fd) != m_map.end());

        timeSets.remove(m_map[fd]);
    }

    friend class SocketHandler;

    template<class T> friend   class TCPAcceptor;
};

inline void SocketHandler::attach()
{
    DEBUG << "Attach Handler for socket: " << m_sock;
    assert(m_loop && m_sock.stat());
    m_loop->attachHandler(m_sock.get_fd(), this);
}

inline void SocketHandler::detach()
{
    DEBUG << "Detach Handler for socket: " << m_sock;
    assert(m_loop && m_sock.stat());
    if(m_loop->existTimer(m_sock.get_fd()))
    {
        DEBUG << "Exist Timer for socket: " << m_sock;
        m_loop->deleteTimer(m_sock.get_fd());
    }
    m_loop->detachHandler(m_sock.get_fd(), this);
}

inline void SocketHandler::waitTimer(int second)
{
    DEBUG << "Register Event " << "Timer for: " << m_sock;
    assert(m_loop && m_sock.stat());
    m_loop->waitTimer(m_sock.get_fd(), second);
}

inline void SocketHandler::waitTimer(const TimeStamp &tms)
{
    DEBUG << "Register Event " << "Timer for: " << m_sock;
    assert(m_loop && m_sock.stat());
    m_loop->waitTimer(m_sock.get_fd(), tms);
}

inline void SocketHandler::registerRead()
{
    DEBUG << "Register Event " << "Timer for: " << m_sock;
    assert(m_loop && m_sock.stat());
    m_loop->registerRead(m_sock.get_fd());
}

template<class T>
inline void TCPAcceptor<T>::setListenSocket()
{
    DEBUG << "setListenSocket";
    m_loop->m_listenSocket = getSocket().get_fd();
}

#endif