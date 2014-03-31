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

#include "ReactorImpl.h"
#include "ReactorSelect.h"
#include "ReactorEPoll.h"

#define EV_READ   (1<<0)
#define EV_WRITE  (1<<1)
#define EV_TIMER  (1<<2)
#define EV_SIGNAL (1<<3)

class EventLoop
{
    EventLoop(const EventLoop &);
    EventLoop& operator = (const EventLoop &);

    map<int, SocketHandler*> m_map;

    bool   m_quitFlag;

    TimeEventSet timeSets;
    Mutex  m_lock;
    ReactorImpl *m_selector;
public:
    EventPool m_pool;
    int    m_listenSocket;

public:
    EventLoop(const char *name = "ReactorSelect", int thrnum = 2) : m_quitFlag(false), m_pool(thrnum), m_listenSocket(-1)
    {
        if(strcmp(name, "ReactorSelect") == 0)
            m_selector = new ReactorSelect(this);
        else
            m_selector = new ReactorEPoll(this);
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
        TimeStamp next = fireTimer();
        m_selector.dispatch(&next);
    }

    void attachHandler(int fd, SocketHandler *p)
    {
        ScopeMutex scope(&m_lock);

        assert(m_map.find(fd) == m_map.end());

        m_map[fd] = p;
    }

    void detachHandler(int fd, SocketHandler *p)
    {
        ScopeMutex scope(&m_lock);

        assert(m_map.find(fd) != m_map.end());
        assert(m_map[fd] == p);
        assert(m_map.erase(fd) == 1);
    }

private:
    void waitTimer(int fd, uint64_t second)
    {
        ScopeMutex scope(&m_lock);

        assert(m_map.find(fd) != m_map.end());

        TimeStamp tms(second*1000000);
        SocketHandler *handler = m_map[fd];
        timeSets.insert(TimeEventItem(handler, TimeStamp::now() + tms));
    }

    void waitTimer(int fd, const TimeStamp &tms)
    {
        ScopeMutex scope(&m_lock);

        assert(m_map.find(fd) != m_map.end());

        SocketHandler *handler = m_map[fd];
        timeSets.insert(TimeEventItem(handler, TimeStamp::now() + tms));
    }

    void registerRead(int fd)
    {
        ScopeMutex scope(&m_lock);
        assert(m_map.find(fd) != m_map.end());
        m_selector -> registerEvent(m_map[fd], EV_READ);
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

        INFO << count << " Timer Event would be fired";

        Callback<void> call;
        for(int i=0; i<count; i++)
        {
            call = Callback<void>(fires[i]->ptr,&SocketHandler::TimerEvent);
            m_pool.insert(call);
        }

        if(count != 0) timeSets.removen(count);

        if (timeSets.empty())
            return TimeStamp::none();
        else
        {
            TimeStamp tms = iter.first()->timer;

            TimeStamp tms2 = tms - TimeStamp::now();

            if (tms2 > TimeStamp::usecs(0LL)) return tms2;
            else return TimeStamp::usecs(0LL);
        }
    }

    bool existTimer(int fd)
    {
        assert(m_map.find(fd) != m_map.end());

        return timeSets.exist(m_map[fd]);
    }

    void deleteTimer(int fd)
    {
        assert(m_map.find(fd) != m_map.end());

        timeSets.remove(m_map[fd]);
    }

    friend class SocketHandler;

    template<class T> friend  class TCPAcceptor;

public:
    void addActive(int fd,int type)
    {
        assert(m_map.find(fd) != m_map.end());
        if((type & EV_READ) != 0)
        {
            Callback<void> call(*m_map[fd], &SocketHandler::onReceiveMsg);
            m_selector->unRegisterEvent(m_map[fd],EV_READ);
            m_pool.insert(call);
        }

        if((type & EV_WRITE) != 0)
        {
            Callback<void> call(*m_map[fd], &SocketHandler::onSendMsg);
            m_selector->unRegisterEvent(m_map[fd],EV_WRITE);
            m_pool.insert(call);
        }
    }
};

inline void SocketHandler::attach()
{
    INFO << "Attach Handler for socket: " << getSocket();
    assert(m_loop && getSocket().stat());
    m_loop->attachHandler(getSocket().get_fd(), this);
}

inline void SocketHandler::detach()
{
    INFO << "Detach Handler for socket: " << getSocket();
    assert(m_loop && getSocket().stat());
    if(m_loop->existTimer(getSocket().get_fd()))
        m_loop->deleteTimer(getSocket().get_fd());
    m_loop->detachHandler(getSocket().get_fd(), this);
}

inline void SocketHandler::waitTimer(int second)
{
    INFO << "Register " << "Timer for: " << getSocket();
    assert(m_loop && getSocket().stat());
    m_loop->waitTimer(m_sock.get_fd(), second);
}

inline void SocketHandler::waitTimer(const TimeStamp &tms)
{
    INFO << "Register " << "Timer for: " << getSocket();
    assert(m_loop && m_sock.stat());
    m_loop->waitTimer(m_sock.get_fd(), tms);
}

inline void SocketHandler::registerRead()
{
    INFO << "Register " << "Timer for: " << m_sock;
    assert(m_loop && m_sock.stat());
    m_loop->registerRead(m_sock.get_fd());
}

template<class T>
inline void TCPAcceptor<T>::setListenSocket()
{
    INFO << "TCPAcceptor SetListenSocket: " << getSocket();
    m_loop->m_listenSocket = getSocket().get_fd();
}

inline void ReactorSelect::dispatch(TimeStamp next)
{
    fd_set rr,ww;
    FD_ZERO(&rr);FD_ZERO(&ww);

    {
        ScopeMutex scope(&m_mutex);
        rr = m_readfds;
        ww = m_writefds;
    }
    
    if (next)
    {
        struct timeval tv = next.to_timeval();
        DEBUG << "Calling ::select(): waiting " << next.to_msecs() << "ms";
        num = ::select(m_mfd+1, &rr, &ww, 0, &tv);
    }
    else
    {
        next = TimeStamp(5000000);
        struct timeval tv = next.to_timeval();
        DEBUG << "Calling ::select() (at most 5 seconds)";
        num = ::select(m_mfd+1, &rr, &ww, 0, &tv);
    }

    {
        ScopeMutex scope(&m_mutex);
        int curnum = 0, i;
        for (i = 0; i <= m_mfd && curnum < num; ++i)
        {
            if(FD_ISSET(i, &rr) || FD_ISSET(i, &ww))
                curnum++;
            if (FD_ISSET(i, &rr))
                m_loop->addActive(i,0);
            if(FD_ISSET(i, &ww))
                m_loop->addActive(i,1);
        }

        for (i = m_maxfd; i >= 0; i--)
        {
            if(FD_ISSET(i, &m_readfds) == true)
            {
                m_maxfd = i; break;
            }
            else if(FD_ISSET(i, &m_writefds) == true)
            {
                m_maxfd = i; break;
            }
        }

        if(i == 0) m_maxfd = -1;
    }
}

inline void ReactorEPoll::dispatch(TimeStamp next)
{
    int num;
    if (next)
    {
        struct timeval tv = next.to_timeval();
        DEBUG << "Calling ::select(): waiting " << next.to_msecs() << "ms";
        num = epoll_wait(m_epollFD, m_pEvents, m_iEvents, tv);
    }
    else
    {
        next = TimeStamp(5000000);
        struct timeval tv = next.to_timeval();
        DEBUG << "Calling ::select() (at most 5 seconds)";
        num = epoll_wait(m_epollFD, m_pEvents, m_iEvents, tv);
    }

    for(int i = 0; i < res; i++)
    {
        int what = m_pEvents[i].events;
        int events = 0;
        SocketHandler *handler = (SocketHandler*)m_pEvents[i].data.ptr;

        if(what & (EPOLLHUP|EPOLLERR))
        {
            events = EV_READ|EV_WRITE;
            assert(0);
        }
        else
        {
            if(what & EPOLLIN ) events |= EV_READ;
            if(what & EPOLLOUT) events |= EV_WRITE;
        }
        if(events != 0) m_loop->addActive(handler->getSocket(), events);
    }
}

#endif