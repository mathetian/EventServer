#ifndef _MSG_HANDLER_H
#define _MSG_HANDLER_H

#include "../utils/Slice.h"
#include "SocketHandler.h"

class MSGHandler : public SocketHandler 
{
protected:
    MSGHandler(EventLoop& loop, Socket sock) : SocketHandler(loop, sock)
    { }

    MSGHandler(EventLoop& loop) : SocketHandler(loop)
    { }

    virtual ~MSGHandler() {}

private:
    bool write(const Slice& buf) 
    {
        const void *data = buf;
        uint32_t length  = buf.length();

        if (length > 0 && wbuf.empty()) 
        {
            Socket sock = getSock();
            int written = sock.write(data, length);

            if (written > 0) 
            {
                data = static_cast<const char *>(data) + written;
                length -= written;
            }
            //zero ?

            if (written < 0 && errno != EAGAIN)
                return false;
        }

        if (length > 0) 
        {
            wbuf.append(static_cast<const char *>(data), length);
            waitWrite(true);
        }

        return true;
    }

    bool write(string s) 
    {
    	return write(s.c_str());
    }

    //Todo List
public:
    bool is_connected() 
    { 
      return getSock().getpeername(); 
    }

    virtual void OnReceiveMsg()  = 0;
    virtual void onSendMsg()     = 0;
    virtual void onCloseSocket() = 0;

private:
    string rbuf, wbuf;
};

#endif