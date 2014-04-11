#ifndef _RECTOR_EPOLL_H
#define _RECTOR_EPOLL_H

#include <sys/epoll.h>
#include "SocketHandler.h"

#define MAX_NEVENTS 1000000

class Selector
{
public:
    Selector(EventLoop* loop) : m_loop(loop)
    {
        assert((m_epollfd = epoll_create(1)) != -1);
        m_events = (struct epoll_event*)malloc(MAX_NEVENTS * sizeof(struct epoll_event));
        assert(m_events != NULL);
        m_scknum = 0;
    }

    virtual void registerEvent(SocketHandler *handler, short event)
    {
        Socket sock = handler->getSocket();
        int fd = sock.get_fd(), events = event, addFlag = 1;

        if(handler->getStatus() != 0) addFlag = 0;

        handler->updateStatus(event);
        events = handler->getStatus();

        //events |= EPOLLET;

        struct epoll_event epev = {0, {0}};
        epev.events   = events;
        epev.data.ptr = handler;

        if(addFlag == 1)
        {
            assert(epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &epev) == 0);
            m_scknum++;
        }
        else
            assert(epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &epev) == 0);
    }

    virtual void unRegisterEvent(SocketHandler *handler, short event)
    {
        Socket sock = handler->getSocket();
        int fd = sock.get_fd(), events = 0, delflag = 1;

        if(handler->getdelflag() == 1) return;

        if(event == -1)
        {
            DEBUG << "found Close Event(triggered by detach), fd: " << fd;
            if(epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL) != 0)
            {
                printf("error = %s, %d\n", strerror(errno), fd);
                assert(0);
            }
            handler->setdelflag();
            return;
        }
        else if((event & EPOLLRDHUP) || (event & EPOLLERR) || (event & EPOLLHUP))
        {
            if (event & EPOLLRDHUP)
                DEBUG << "found EPOLLRDHUP for fd: " << fd;
            else
                DEBUG << "found EPOLLERR for fd: " << fd;

            assert(epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL) == 0);
            handler->setdelflag();
            return;
        }

        handler->removeStatus(event);
        events = handler->getStatus();
        if(events != 0) delflag = 0;

        //events |= EPOLLET;

        if(delflag == 0)
        {
            struct epoll_event epev = {0, {0}};
            epev.events   = events;
            epev.data.ptr = handler;
            assert(epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &epev) == 0);
        }
        else if(delflag == 1)
        {
            assert(epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL) == 0);
            m_scknum--;
        }
    }

    virtual int dispatch();

private:
    int m_epollfd;
    struct epoll_event *m_events;
    int m_scknum;
    EventLoop *m_loop;
};

#endif