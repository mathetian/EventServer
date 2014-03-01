#ifndef _EVENT_LOOP_H
#define _EVENT_LOOP_H

#include <map>
using namespace std;

#include "SocketHandler.h"

class EventLoop 
{
    /**Prevent copy-construct and equal construct**/
    EventLoop(const EventLoop &);
    EventLoop& operator = (const EventLoop &);

    map<int, SocketHandler*> m_map;

    fd_set m_readfds;
    fd_set m_writefds;
    int    m_maxfd;
    bool   m_quitFlag;

public:
    EventLoop() : m_quitFlag(false)
    {
        FD_ZERO(&m_readfds);
        FD_ZERO(&m_writefds);
        m_maxfd = -1;
    }

    ~EventLoop() 
    {
        map<int, SocketHandler*>::iterator iter = m_map.begin();
        for(;iter != m_map.end();iter++)
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
        if (m_quitFlag) return;

        fd_set rr, ww;

        rr = m_readfds;
        ww = m_writefds;  

        //don't support OOB
       ::select(m_maxfd+1, &rr, &ww, 0, 0);

        for (int i = 0; i <= m_maxfd; ++i) 
        {
            if (m_map.find(i) != m_map.end() && FD_ISSET(i, &rr)) 
            {
                m_map[i]->onReceiveMsg();
            }
            if (m_map.find(i) != m_map.end() && FD_ISSET(i, &ww)) 
            {
                m_map[i]->onSendMsg();
            }
        }
    }

    void attachHandler(int fd, SocketHandler *p) 
    {
        assert(fd >= 0 && fd < FD_SETSIZE);
        assert(m_map.find(fd) == m_map.end());
        assert(p);

        m_map[fd] = p;

        m_maxfd = max(m_maxfd, fd);

        bool r = p->m_waitRead;
        bool w = p->m_waitWrite;

        if (r) FD_SET(fd, &m_readfds); 
        else   FD_CLR(fd, &m_readfds);
        
        if (w) FD_SET(fd, &m_writefds); 
        else   FD_CLR(fd, &m_writefds);

    }

    void detachHandler(int fd, SocketHandler *p)
    {
        assert(fd >= 0 && fd < FD_SETSIZE);
        assert(m_map.find(fd) != m_map.end());
        assert(p);
        assert(m_map[fd] == p);

        assert(m_map.erase(fd) == 1);

        if(m_maxfd == fd) m_maxfd--;

        FD_CLR(fd, &m_readfds);
        FD_CLR(fd, &m_writefds);
    }

private:
    void waitRead(int fd, bool act) 
    {
        assert(fd >= 0 && fd < FD_SETSIZE);

        if (act)
        {
            m_maxfd = max(m_maxfd, fd);
            FD_SET(fd, &m_readfds);
        }
        else
        {
            assert(fd <= m_maxfd);
            FD_CLR(fd, &m_readfds);
        }
    }

    void waitWrite(int fd, bool act) 
    {
        assert(fd >= 0 && fd < FD_SETSIZE);

        if (act)
        {
            FD_SET(fd, &m_writefds);
            m_maxfd = max(m_maxfd, fd);
        }
        else
        {
            assert(fd <= m_maxfd);
            FD_CLR(fd, &m_writefds);
        }
    }

    friend class SocketHandler;
};

inline void SocketHandler::attach()
{
    assert(m_loop && m_sock.stat());
    m_loop->attachHandler(m_sock.get_fd(), this);
}

inline void SocketHandler::detach()
{
    assert(m_loop && m_sock.stat());
    m_loop->detachHandler(m_sock.get_fd(), this);
}

inline void SocketHandler::waitRead(bool act)
{
    assert(m_loop && m_sock.stat());
    m_waitRead = act;   
    m_loop->waitRead(m_sock.get_fd(), act);
}

inline void SocketHandler::waitWrite(bool act)
{
    assert(m_loop && m_sock.stat());
    m_waitWrite = act;
    m_loop->waitWrite(m_sock.get_fd(), act);
}

#endif