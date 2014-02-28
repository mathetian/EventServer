#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include "SocketHandler.h"

template<class T>
class TCPAcceptor : private SocketHandler 
{
    TCPSocket m_sock;

public:
    TCPAcceptor(EventLoop& _loop, int localport) :
        SocketHandler(_loop), m_sock(NetAddress(localport), Socket::acceptor)
    {
        setSock(m_sock);
        m_sock.set_blocking(false);
        waitRead(true);waitWrite(true);
    }

    TCPAcceptor(EventLoop& _loop, string ip, int localport) :
        SocketHandler(_loop), m_sock(NetAddress(ip, localport), Socket::acceptor)
    {
        setSock(m_sock);
        m_sock.set_blocking(false);
        waitRead(true);waitWrite(true);
    }

    virtual ~TCPAcceptor() {}
 
private:
    void readAvail() 
    {
        NetAddress a;
        TCPSocket sock = m_sock.accept(a);
        
        if (sock) 
        {
            T* t = new T(*getLoop(), sock);
        }
    }
};

#endif