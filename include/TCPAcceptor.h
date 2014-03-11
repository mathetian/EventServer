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
        DEBUG << "TCPAcceptor Initialiaztion Successfully";
        assert(m_sock.stat());
        INFO << "Server Information: " << m_sock.getsockname();
        attach();
        setListenSocket();
        registerRead();
    }

    TCPAcceptor(EventLoop& _loop, string ip, int localport) : SocketHandler(_loop)
    {
        NetAddress addr = NetAddress(ip,localport);
        setSock(TCPSocket(&addr, Socket::acceptor));
        DEBUG << "TCPAcceptor Initialiaztion Successfully";
        DEBUG << "Acceptor Socket STATUS: " << m_sock.stat();
        assert(m_sock.stat());
        INFO << "Server Information: " << m_sock.getsockname();
        attach();
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
    void onSendMsg()
    {
        DEBUG << "NO NEED";
    }
    void onCloseSocket()
    {
        DEBUG << "NO NEED";
    }
    void TimerEvent()
    {
        DEBUG << "NO NEED";
    }

private:
    void setListenSocket();
};

#endif