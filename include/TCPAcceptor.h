#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include "SocketHandler.h"

#include "../utils/Log.h"

template<class T>
class TCPAcceptor : public SocketHandler 
{
public:
    TCPAcceptor(EventLoop& _loop, int localport) : SocketHandler(_loop)
    {
        NetAddress addr = NetAddress(localport);
        setSock(TCPSocket(&addr, Socket::acceptor));
        DEBUG << "Acceptor Socket STATUS: " << m_sock.stat();
        assert(m_sock.stat());
        DEBUG << "TCPAcceptor Initialiaztion Successfully";
        
        INFO << "Server Information: " << m_sock.getsockname();
        attach();
        waitRead(true);
    }

    TCPAcceptor(EventLoop& _loop, string ip, int localport) : SocketHandler(_loop)
    {
        NetAddress addr = NetAddress(ip,localport);
        setSock(TCPSocket(&addr, Socket::acceptor));
        DEBUG << "Acceptor Socket STATUS: " << m_sock.stat();
        assert(m_sock.stat());
        DEBUG << "TCPAcceptor Initialiaztion Successfully";

        INFO << "Server Information: " << m_sock.getsockname();
        attach();
        waitRead(true);
    }

    virtual ~TCPAcceptor() {}
 
private:
    void onReceiveMsg() 
    {
        NetAddress a;
        TCPSocket sock = m_sock.accept(&a);
        if (sock) 
        {
           T* t = new T(*getLoop(), sock);
        }
    }
    /**without any implementation**/
    void onSendMsg() { }
    void onCloseSocket() { }
    void onTimer() { }
};

#endif