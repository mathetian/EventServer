#ifndef _ADDRESS_H
#define _ADDRESS_H

#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
using namespace std;

#include "../utils/Utils.h"
using namespace utils;

class Address
{
protected:
    string m_addr;

public:
    Address() {}

    Address(const void *addr, socklen_t len) : m_addr(static_cast<const char*>(addr), len)
    { }

    const sockaddr *data() const
    {
        return static_cast<const sockaddr *>(static_cast<const void *>(m_addr.data()));
    }

    socklen_t length() const
    {
        return m_addr.length();
    }

    void   setAddr(const void *addr, socklen_t len)
    {
        m_addr = string(static_cast<const char *>(addr), len);
    }
};

class NetAddress : public Address
{
public:
    typedef uint32_t port;

private:
    NetAddress(const Address& other);
    NetAddress& operator = (const NetAddress &);

private:
    void init(string ip, port pt)
    {
        sockaddr_in a;
        hostent *ent;

        a.sin_family = AF_INET;
        a.sin_port   = htons(pt);

        if ((ent = gethostbyname(ip.c_str())) != NULL)
        {
            memcpy(&a.sin_addr.s_addr, ent->h_addr, ent->h_length);
            m_addr = string(static_cast<const char *>(static_cast<const void *>(&a)), sizeof a);
        }
    }

    const sockaddr_in *inetAddr() const
    {
        return static_cast<const sockaddr_in *>\
               (static_cast<const void *>(data()));
    }

public:
    NetAddress() { }
    NetAddress(port pt)
    {
        sockaddr_in a;
        a.sin_family = AF_INET;
        a.sin_port   = htons(pt);
        a.sin_addr.s_addr = INADDR_ANY;

        m_addr = string(static_cast<const char *>(static_cast<const void *>(&a)), sizeof a);
    }

    NetAddress(string ip, port pt)
    {
        init(ip, pt);
    }

    NetAddress(const void *addr, socklen_t len) : Address(addr, len) { }

    string getIP() const
    {
        return inet_ntoa(inetAddr()->sin_addr);
    }

    port getPort() const
    {
        return ntohs(inetAddr()->sin_port);
    }

    virtual string as_string() const
    {
        string out = getIP();

        out += ":" + to_string(getPort());

        return out;
    }
};

TO_STRING(NetAddress);

#endif