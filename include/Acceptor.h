#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include "EventHandler.h"

template<class T>
class TCPAcceptor : private SocketHandler 
{
    TCPSocket m_sock;

public:
    TCPAcceptor(EventLoop& _loop, in_port_t localport) :
        SocketHandler(_loop), m_sock(NetAddress(localport), Socket::acceptor)
    {
        setSock(m_sock);
        m_sock.set_blocking(false);
        want_read(true);
    }

    virtual ~TCPAcceptor() {}
 
private:
    void readAvail() 
    {
        Address a;
        TCPSocket sock = m_sock.accept(a);
        if (sock) 
        {
            T* t = new T(*getLoop(), sock);
        }
    }
};

#endif