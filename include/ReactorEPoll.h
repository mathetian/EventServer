#ifndef _RECTOR_EPOLL_H
#define _RECTOR_EPOLL_H

#include <sys/epoll.h>

#include "ReactorImpl.h"

class ReactorEPoll : public ReactorImpl
{
public:
	ReactorEPoll(const EventLoop*loop) : ReactorImpl(loop) 
	{ 
		assert((m_epollFD = epoll_create(0)) != -1);

	}
	virtual int Finalize() { }
	virtual const char *GetMethod() { return "ReactorEPoll"; }
	virtual int RegisterEvent(SocketHandler *handler, short event) 
	{
		Socket sock = handler->getSocket();
		int events = 0, op;

		op = EPOLL_CTL_ADD;
		events = 0;

		if(event & EV_READ) events  |= EPOLLIN;
		if(event & EV_WRITE) events |= EPOLLOUT;

		struct epoll_event epev = {0, {0}};
		epev.events = events;
		if(epoll_ctl(m_epollFD, op, sock, &epev) == -1)
		{
			switch(errno)
			{
				case EEXIST: 
					return 1;
			}
			return 0;
	    }
		return 1;
	}
	virtual int UnregisterEvent(SocketHandler *handler, short event)
	{
		Socket sock = handler->getSocket();
		int op = EPOLL_CTL_DEL, events = 0;

		struct epoll_event epev = {0, {0}};
	    epev.events = events;
	    if(epoll_ctl(m_epollFD, op, sock, &epev) == -1)
	    {
			if(op == EPOLL_CTL_DEL)
			{
				switch(errno)
				{
					case EBADF: // bad file descriptor
					case ENOENT: // the fd is already removed
					return 0;
				}
			}
	          
	         return (-1); 
		}
		return (0);
	}
	virtual int Dispatch(struct timeval *tv);

private:
	int m_epollFD;
};

#endif