#ifndef _RECTOR_IMPL_H
#define _RECTOR_IMPL_H

#include "SocketHandler.h"

#define EV_READ   (1<<0)
#define EV_WRITE  (1<<1)
#define EV_CLOSE  (1<<2)

class ReactorImpl{
public:
	ReactorImpl(EventLoop* const loop) : m_loop(loop) { };
	virtual int dispatch(TimeStamp next) = 0;
	virtual const char *getMethod() = 0;
	virtual int registerEvent(SocketHandler *handler, short event) = 0;
	virtual int unRegisterEvent(SocketHandler *handler, short event) = 0;

protected:
	EventLoop * const m_loop;
};
#endif