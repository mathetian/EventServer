#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include "Log.h"
#include "SocketHandler.h"

namespace sealedServer
{

template<class T>
class TCPAcceptor : public SocketHandler
{
public:
    TCPAcceptor(EventLoop& _loop, int localport) : SocketHandler(_loop)
    {
        NetAddress addr = NetAddress(localport);
        setSock(TCPSocket(&addr, Socket::acceptor));
        INFO << "TCPAcceptor Initialiaztion";
        INFO << "Server Information: " << getSocket().getsockname();
        assert(getSocket().stat());

        attach();
        setListenSocket();

        registerRead();
    }

    TCPAcceptor(EventLoop& _loop, string ip, int localport) : SocketHandler(_loop)
    {
        NetAddress addr = NetAddress(ip,localport);
        setSock(TCPSocket(&addr, Socket::acceptor));
        INFO << "TCPAcceptor Initialiaztion";
        INFO << "Acceptor Socket STATUS: " << getSocket().stat();
        assert(m_sock.stat());
        attach();
    }

    virtual ~TCPAcceptor() {}

private:
    void onReceiveMsg()
    {
        NetAddress a;
        TCPSocket sock = m_sock.accept(&a);

        if (sock) T* t = new T(*getLoop(), sock);
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

};

#endif