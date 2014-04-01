#ifndef _RECTOR_EPOLL_H
#define _RECTOR_EPOLL_H

#include <sys/epoll.h>

#include "../../utils/Thread.h"
#include "../../utils/Log.h"
using namespace utils;

#include "../ReactorImpl.h"

#define MAX_NEVENTS 1000000

class ReactorEPoll : public ReactorImpl
{
public:
	ReactorEPoll(EventLoop* const loop) : ReactorImpl(loop) 
	{ 
		assert((m_epollFD = epoll_create(1)) != -1);
		m_pEvents = (struct epoll_event*)malloc(MAX_NEVENTS * sizeof(struct epoll_event));
		assert(m_pEvents != NULL);
		m_scknum = 0;
	}
	virtual const char *getMethod() { return "ReactorEPoll"; }
	virtual int registerEvent(SocketHandler *handler, short event) 
	{
		Socket sock = handler->getSocket();
		int fd = sock.get_fd(), events = 0, addFlag = 1;

		if(handler->getStatus() != 0) addFlag = 0;
		
		handler->updateStatus(event);
		event = handler->getStatus();

		if(event & EV_READ)  events |= EPOLLIN;
		if(event & EV_WRITE) events |= EPOLLOUT;
		events |= EPOLLET;

		struct epoll_event epev = {0, {0}};
		epev.events   = events;
		epev.data.ptr = handler;
		
		ScopeMutex scope(&m_lock);
		if(addFlag == 1)
		{
			assert(epoll_ctl(m_epollFD, EPOLL_CTL_ADD, fd, &epev) == 0);
			m_scknum++;
		}
		else
		{
			assert(epoll_ctl(m_epollFD, EPOLL_CTL_MOD, fd, &epev) == 0);
		}
		
		return 1;
	}

	virtual int unRegisterEvent(SocketHandler *handler, short event)
	{
		Socket sock = handler->getSocket();
		int fd = sock.get_fd(), events = 0, delflag = 1;

		if(event == EV_CLOSE)
		{
			assert(epoll_ctl(m_epollFD, EPOLL_CTL_DEL, fd, NULL) == 0);
			return 1;
		}
		
		handler->removeStatus(event);
		event = handler->getStatus();
		if(event != 0) delflag = 0;

		if(event & EV_READ)  events |= EPOLLIN;
		if(event & EV_WRITE) events |= EPOLLOUT;
		events |= EPOLLET;
		
		ScopeMutex scope(&m_lock);
		if(delflag == 0)
		{
			struct epoll_event epev = {0, {0}};
			epev.events   = events;
			epev.data.ptr = handler;
			assert(epoll_ctl(m_epollFD, EPOLL_CTL_MOD, fd, &epev) == 0);
		}
		else if(delflag == 1)
		{
			assert(epoll_ctl(m_epollFD, EPOLL_CTL_DEL, fd, NULL) == 0);
			m_scknum--;
		}
		
		return 1;
	}
	virtual int dispatch(TimeStamp next);

private:
	int m_epollFD;
	struct epoll_event *m_pEvents;
	int m_scknum;
	Mutex m_lock;
};

#endif