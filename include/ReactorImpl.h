#ifndef _RECTOR_IMPL_H
#define _RECTOR_IMPL_H

#include "SocketHandler.h"

class ReactorImpl{
public:
	ReactorImpl(const EventLoop*loop) : m_loop(loop) { };
	virtual int sispatch(TimeStamp tv) = 0;
	virtual const char *getMethod() = 0;
	virtual int registerEvent(SocketHandler *handler, short event) = 0;
	virtual int unRegisterEvent(SocketHandler *handler, short event) = 0;

protected:
	const EventLoop *m_loop;
};
#endif