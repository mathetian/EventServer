#ifndef _RECTOR_EPOLL_H
#define _RECTOR_EPOLL_H

#include <sys/epoll.h>

#include "../../utils/Thread.h"
#include "../../utils/Log.h"
using namespace utils;

#include "../ReactorImpl.h"

#define MAX_NEVENTS 4096

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
		int fd = sock.get_fd();

		int events = 0;
		
		if(event & EV_READ)  events |= EPOLLIN;
		if(event & EV_WRITE) events |= EPOLLOUT;

		struct epoll_event epev = {0, {0}};
		epev.events   = events;
		epev.data.ptr = handler;
		
		ScopeMutex scope(&m_lock);
		assert(epoll_ctl(m_epollFD, EPOLL_CTL_ADD, fd, &epev) == 0);
		m_scknum++;
		
		return 1;
	}

	virtual int unRegisterEvent(SocketHandler *handler, short event)
	{
		Socket sock = handler->getSocket();
		int fd = sock.get_fd();

		ScopeMutex scope(&m_lock);
		assert(epoll_ctl(m_epollFD, EPOLL_CTL_DEL, fd, NULL) == 0);
		m_scknum--;

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