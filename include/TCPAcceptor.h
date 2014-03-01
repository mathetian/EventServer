#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include "SocketHandler.h"

#include "../utils/Log.h"

template<class T>
class TCPAcceptor : public SocketHandler 
{
    TCPSocket m_sock;

public:
    TCPAcceptor(EventLoop& _loop, int localport) :
        m_sock(NetAddress(localport), Socket::acceptor)
    {
        SocketHandler::SocketHandler(_loop, m_sock, true);
        DEBUG << "Socket status: ";
        assert(m_sock.stat());
        DEBUG << "TCPAcceptor Initialiaztion Successfully";
    }

    TCPAcceptor(EventLoop& _loop, string ip, int localport) :
        SocketHandler(_loop), m_sock(NetAddress(ip, localport), Socket::acceptor)
    {
        assert(m_sock.stat());
        waitRead(true);
    }

    virtual ~TCPAcceptor() {}
 
private:
    void onReceiveMsg() 
    {
        NetAddress a;
        TCPSocket sock = m_sock.accept(a);
        
        if (sock) 
        {
           T* t = new T(*getLoop(), sock);
        }
    }
    /**without any implementation**/
    void onSendMsg() { }
    virtual void onCloseSocket() { }
};

#endif