#ifndef _RECTOR_EPOLL_H
#define _RECTOR_EPOLL_H

#include <sys/epoll.h>

#include "../ReactorImpl.h"

#define MAX_NEVENTS 4096

class ReactorEPoll : public ReactorImpl
{
public:
	ReactorEPoll(EventLoop* const loop) : ReactorImpl(loop) 
	{ 
		assert((m_epollFD = epoll_create(0)) != -1);
		m_pEvents = (struct epoll_event*)malloc(MAX_NEVENTS * sizeof(struct epoll_event));
		assert(m_pEvents != NULL);
		m_iEvents = MAX_NEVENTS;
	}
	virtual const char *getMethod() { return "ReactorEPoll"; }
	virtual int registerEvent(SocketHandler *handler, short event) 
	{
		Socket sock = handler->getSocket();
		int events = 0;
		
		if(event & EV_READ)  events |= EPOLLIN;
		if(event & EV_WRITE) events |= EPOLLOUT;

		struct epoll_event epev = {0, {0}};
		epev.events = events;
		epev.data.ptr = handler;
		
		assert(epoll_ctl(m_epollFD, EPOLL_CTL_ADD, sock, &epev) == 0);
		return 1;
	}
	virtual int unRegisterEvent(SocketHandler *handler, short event)
	{
		Socket sock = handler->getSocket();
		int op = EPOLL_CTL_DEL, events = 0;

		if(event & EV_READ)  events |= EPOLLIN;
		if(event & EV_WRITE) events |= EPOLLOUT;

		struct epoll_event epev = {0, {0}};
	    epev.events = events;
	    epev.data.ptr = handler;

	    if(epoll_ctl(m_epollFD, EPOLL_CTL_DEL, sock, &epev) == -1)
	    {
			switch(errno)
			{
				case EBADF: // bad file descriptor
				case ENOENT: // the fd is already removed
				return 0;
			}
	         return (-1); 
		}
		return (0);
	}
	virtual int dispatch(TimeStamp next);

private:
	int m_epollFD;
	struct epoll_event *m_pEvents;
	int m_iEvents;

};

#endif