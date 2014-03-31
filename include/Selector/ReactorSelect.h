#ifndef _RECTOR_SEL_H
#define _RECTOR_SEL_H

#include <vector>
using namespace std;

#include "../../utils/Thread.h"
#include "../../utils/Log.h"
using namespace utils;
#include "../ReactorImpl.h"

#include "assert.h"

class ReactorSelect : public ReactorImpl
{
public:
	ReactorSelect(EventLoop* const loop) : ReactorImpl(loop) 
	{ 
		FD_ZERO(&m_readfds);
		FD_ZERO(&m_writefds);
		m_maxfd  = -1;
		m_scknum = 0;
	}

	virtual const char *getMethod() { return "RectorSelect"; }

	virtual int dispatch(TimeStamp next);

	virtual int registerEvent(SocketHandler *handler, short event)
	{
		Socket sock = handler->getSocket();
		int fd = sock.get_fd();

		if(event == EV_READ) 
		{
			assert(FD_ISSET(fd, &m_readfds) == false);
			FD_SET(fd,&m_readfds);
		}
		else
		{
			assert(FD_ISSET(fd, &m_writefds) == false);
			FD_SET(fd,&m_writefds);
		}
		ScopeMutex scope(&m_lock2);
		m_maxfd = max(m_maxfd, fd);
		m_scknum++;
	}

	virtual int unRegisterEvent(SocketHandler *handler, short event)
	{
		Socket sock = handler->getSocket();
		int fd = sock.get_fd();
		if(event == EV_READ) 
		{
			assert(FD_ISSET(fd, &m_readfds) == true);
			FD_CLR(fd,&m_readfds);
		}
		else
		{
			assert(FD_ISSET(fd, &m_writefds) == true);
			FD_CLR(fd,&m_writefds);
		}

		ScopeMutex scope(&m_lock2);
		int i;
		for(i=m_maxfd;i>=0;i--)
		{
			if(FD_ISSET(i,&m_readfds) || FD_ISSET(i,&m_writefds))
				break;
		}
		m_maxfd = i;
		m_scknum--;
	}

private:
	int       m_maxfd;
	int       m_scknum; 
	fd_set    m_readfds;
	fd_set    m_writefds;
	Mutex     m_lock;
	Mutex     m_lock2;
};


#endif