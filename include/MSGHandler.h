#ifndef _MSG_HANDLER_H
#define _MSG_HANDLER_H

#include "../utils/Slice.h"
#include "EventHandler.h"

class MSGHandler : private SocketHandler 
{
    bool established;

protected:
    MSGHandler(EventLoop& loop, Socket sock, bool established = false) : SocketHandler(loop)
    {
	     set_socket(sock, established);
    }

    MSGHandler(EventLoop& loop) : SocketHandler(loop), established(false)
    {
    }

    virtual ~MSGHandler() {}

    virtual void connected(Status stat) {}

    virtual int incoming_data(const char* buf) = 0;

    virtual void end_data(const char* buf) {}

public:
    bool write(const Slice& buf) 
    {
        const void *data = buf;
        uint32_t length = buf.length();

        if (established && length > 0 && wbuf.empty()) 
        {
            int written = ::send(getSock().get_fd(), data, length, MSG_DONTWAIT | MSG_NOSIGNAL);
            
            if (written > 0) 
            {
                data = static_cast<const char *>(data) + written;
                length -= written;
            }
            if (written < 0 && errno != EAGAIN)
                return false;
        }

        if (length > 0) 
        {
            wbuf.append(static_cast<const char *>(data), length);
            want_write(true);
        }

        return true;
    }

    bool write(string s) 
    {
    	return write(s.c_str());
    }

    Socket get_socket() 
    { 
        return Socket(getSock()); 
    }

    void set_socket(Socket sock, bool established = false) 
    {
      	setSock(sock);
      	this->established = sock && established;

      	if (this->established && sock.stat()) 
        {
      	    want_read(true);
      	    want_write(rbuf.length() != 0);
      	} 
        else if (sock) 
        {
      	    want_read(false);
      	    want_write(true);
      	}
    }

    bool is_connected() 
    { 
      return get_socket().getpeername(); 
    }

private:
    string rbuf, wbuf;

    void readAvail() 
    {
        char buf[512];
        int bytes = recv(getSock().get_fd(), buf, sizeof buf, MSG_DONTWAIT);

        if (bytes == 0 || (bytes < 0 && errno != EAGAIN)) 
        {
            want_read(false);
            end_data(rbuf.c_str());
            return;
        }

        if (bytes > 0) 
        {
            rbuf.append(buf, bytes);
            unsigned int consumed = incoming_data(rbuf.c_str());

            if (consumed > 0) {
                assert(consumed <= rbuf.length());
                rbuf.erase(0, consumed);
            }
        }
    }

    void writeAvail() 
    {
        if (!established) 
        {
            established = true;

            Status stat = get_socket().stat();
            connected(stat);
            if (!stat) 
            {
	           want_write(false);
                return;
	        }

            want_read(true);
        }

        const char *p = wbuf.data();
        int len = wbuf.length();

        while (len > 0) 
        {
            int written = ::send(getSock().get_fd(), p, len, MSG_DONTWAIT | MSG_NOSIGNAL);
            if (written == -1 && errno == EAGAIN) 
            {
                wbuf.erase(0, wbuf.length() - len);
                want_write(true);
                return;
            }

            if (written <= 0) 
            {
                want_write(false);
            }

            p += written;
            len -= written;
        }

        want_write(false);
        wbuf.erase();

        return;
    }
};

#endif