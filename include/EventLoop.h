#ifndef _EVENT_LOOP_H
#define _EVENT_LOOP_H

#include "EventHandler.h"

class EventLoop 
{
    EventLoop(const EventLoop &);
    EventLoop& operator = (const EventLoop &);

    class WakeHandler : public SocketHandler 
    {
        Socket wake_wsck;
    public:
        WakeHandler(EventLoop& loop) : SocketHandler(loop) 
        {
            pair<Socket, Socket> ios = Socket::pipe();

            setSock(ios.first);
            wake_wsck = ios.second;

            ios.first.set_blocking(false);
            ios.second.set_blocking(false);

            want_read(true);
        }

        void wake() 
        {
            assert(wake_wsck);
            wake_wsck.write("A", 1);
        }

        void drain() 
        {
            static char garbage[128];
            while (getSock().read(garbage, sizeof garbage) == sizeof garbage) ;
        }

        void ravail() 
        {
            drain();
        }
    };

    vector<SocketHandler*> handlers;

    fd_set readfds;
    fd_set writefds;
    int maxfd;

    bool quit_flag;
    WakeHandler *waker;

public:
    EventLoop() : quit_flag(false)//, event_loop_id(0) 
    {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        maxfd = -1;

        waker = new WakeHandler(*this);
    }

    ~EventLoop() 
    {
    	delete waker;
    	waker = NULL;
    }
    
    void run() 
    {
        while (!quit_flag) 
        	run_once();
    }

    void run_once()
    {
        if (quit_flag) return;

        fd_set rr, ww;

        rr = readfds;
        ww = writefds;

       ::select(maxfd+1, &rr, &ww, 0, 0);

        for (int i = 0; i <= maxfd; ++i) 
        {
            if (handlers[i] && FD_ISSET(i, &rr)) 
            {
                handlers[i]->readAvail();
            }
            if (handlers[i] && FD_ISSET(i, &ww)) 
            {
                handlers[i]->writeAvail();
            }
        }
    }

    void terminate() 
    {
        quit_flag = true;
        wake();
    }

    void wake() 
    {
    }

private:
    void want_read(int fd, bool act) 
    {
        assert(fd >= 0 && fd < FD_SETSIZE);

        if (act)
            FD_SET(fd, &readfds);
        else
            FD_CLR(fd, &readfds);

        wake();
    }

    void want_write(int fd, bool act) 
    {
        assert(fd >= 0 && fd < FD_SETSIZE);

        if (act)
            FD_SET(fd, &writefds);
        else
            FD_CLR(fd, &writefds);

        wake();
    }

    void set_handler(int fd, SocketHandler *p) 
    {
        assert(fd >= 0 && fd < FD_SETSIZE);

        if (handlers.size() <= unsigned(fd))
            handlers.resize(fd + 1);
        handlers[fd] = p;
        if (fd > maxfd)
            maxfd = fd;

        bool r = p && p->cur_want_read;
        bool w = p && p->cur_want_write;


        if (r) FD_SET(fd, &readfds); else FD_CLR(fd, &readfds);
        if (w) FD_SET(fd, &writefds); else FD_CLR(fd, &writefds);

    }
    
private:
    friend class Socket_Handler;
    friend class timer;
};

#endif