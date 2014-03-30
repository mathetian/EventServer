#ifndef _RECTOR_IMPL_H
#define _RECTOR_IMPL_H

#include "SocketHandler.h"

#define EV_READ   (1<<0)
#define EV_WRITE  (1<<1)
#define EV_TIMER  (1<<2)
#define EV_SIGNAL (1<<3)

class ReactorImpl{
public:
	ReactorImpl(const EventLoop*loop) : m_loop(loop) { };
	virtual ~ReactorImpl() { }
	virtual int Dispatch(struct timeval *tv) = 0;
	virtual const char *GetMethod() = 0;
	virtual int RegisterEvent(SocketHandler *handler, short event) = 0;
	virtual int UnregisterEvent(SocketHandler *handler, short event) = 0;

protected:
	const EventLoop *m_loop;
};
#endif