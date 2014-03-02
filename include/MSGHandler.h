#ifndef _MSG_HANDLER_H
#define _MSG_HANDLER_H

#include "../utils/Slice.h"
#include "SocketHandler.h"

class MSGHandler : public SocketHandler 
{
protected:
    MSGHandler(EventLoop& loop, Socket sock) : SocketHandler(loop)
    { 
        setSock(sock);
        DEBUG << "MSGHandler Socket STATUS: " << m_sock.stat();
        assert(m_sock.stat());
        DEBUG << "MSGHandler Initialiaztion Successfully";    
        DEBUG << "Peer Information: " << m_sock.getpeername();    
        attach();
    }

    virtual ~MSGHandler() {}

protected:
    /**Don't support write less than requirement**/
    int write(const Slice& buf) 
    {
        const void *data = buf;
        uint32_t length  = buf.length();

        if(length > 0)
            return getSocket().write(data, length);
        else
            return 0;
    }

    int write(string s) 
    {
    	return write(s.c_str());
    }

    int read(Slice& buf)
    {   
        void *data = buf;
        uint32_t length = buf.length();
        if(length > 0)
            return getSocket().read(data, length);
        else
            return 0;
    }

    int read(void *data, uint32_t length)
    {
        if(length > 0)
            return getSocket().read(data, length);
        else
            return 0;
    }

    void closeSocket()
    {
        //Todo List
    }

    void deleteMe()
    {
        closeSocket();
        delete this;
    }
    
public:
    bool is_connected() 
    { 
      return getSocket().getpeername(); 
    }
    
    virtual void onCloseSocket() = 0;

};

#endif