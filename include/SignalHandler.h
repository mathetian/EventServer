// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _SIGNAL_HANDLER_H
#define _SIGNAL_HANDLER_H

#include <map>
using namespace std;

#include "MSGHandler.h"

namespace eventserver
{

class SignalOuter
{
public:
	static SignalOuter& Instance()
	{
		static SignalOuter instance;
		return instance;
	}

public:
	pair<Socket, Socket> sockets()
	{
		return m_socks;
	}

private:
	SignalOuter()
	{
		m_socks = Socket::pipe();
	}

private:
	pair<Socket, Socket> m_socks;
};

inline void signalhandler(int signo)
{
	static SignalOuter &outer = SignalOuter::Instance();
	char msg = signo;
	int num = write(outer.sockets().second.fd(), (char*)&msg, 1);
}

class SignalHandler : public MSGHandler
{
public:
    SignalHandler(EventLoop *loop, Socket sock) : MSGHandler(loop, sock) { }

private:
	virtual void received(STATUS status, Buffer &buff)
	{
		assert(status == MSGHandler::SUCC);

		for(int i = 0;i < buff.size();i++)
		{
			int signo = buff[i];
			assert(handlers_.find(signo) != handlers_.end());
			handlers_[signo](signo);
		}
	} 

public:
    virtual void attach(int signo, void (*handler)(int))
    {
    	handlers_[signo] = handler;
    	signal(signo, signalhandler);
    }

private:
	map<int, void (*)(int)> handlers_;
};

};

#endif