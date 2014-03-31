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
		m_maxfd = -1;
	}

	virtual const char *getMethod() { return "RectorSelect"; }

	virtual int dispatch(TimeStamp next);

	virtual int registerEvent(SocketHandler *handler, short event)
	{
		Socket sock = handler->getSocket();
		if(event == EV_READ) 
		{
			INFO << FD_ISSET(sock, &m_readfds);
			assert(FD_ISSET(sock, &m_readfds) == false);
			FD_SET(sock,&m_readfds);
		}
		else
		{
			assert(FD_ISSET(sock, &m_writefds) == false);
			FD_SET(sock,&m_writefds);
		}

		ScopeMutex scope(&m_lock);
		m_maxfd = max(m_maxfd, (int)sock);
	}

	virtual int unRegisterEvent(SocketHandler *handler, short event)
	{
		Socket sock = handler->getSocket();
		if(event == EV_READ) 
		{
			assert(FD_ISSET(sock, &m_readfds) == true);
			FD_CLR(sock,&m_readfds);
		}
		else
		{
			assert(FD_ISSET(sock, &m_writefds) == true);
			FD_CLR(sock,&m_writefds);
		}

		ScopeMutex scope(&m_lock);
		int i;
		for(i=m_maxfd;i>=0;i--)
		{
			if(FD_ISSET(i,&m_readfds) || FD_ISSET(i,&m_writefds))
			{
				m_maxfd = i; break;
			}
		}
		if(i<0) m_maxfd = -1;
	}

private:
	int       m_maxfd; 
	fd_set    m_readfds;
	fd_set    m_writefds;
	Mutex     m_lock;
};


#endif