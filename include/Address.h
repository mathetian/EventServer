#ifndef _ADDRESS_H
#define _ADDRESS_H

#include "../utils/Utils.h"

#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

class Address {
protected:
    string addr;

public:
    Address() {}

    Address(const void *addr, socklen_t len) : addr(static_cast<const char *>(addr), len)
    {
    }

    const sockaddr *data() const { return static_cast<const sockaddr *>(static_cast<const void *>(addr.data())); }

    socklen_t length() const { return addr.length(); }

    const string& data_as_string() const { return addr; }

    operator const void *() const { return addr.length() ? this : 0; }

    string as_string() const;
};

class NetAddress : public Address 
{
public:
    typedef uint32_t port;

protected:
    const sockaddr_in *inetAddr() const 
    { 
    	return static_cast<const sockaddr_in *>\
    		(static_cast<const void *>(data())); 
    }

private:
    void init(string ip, port pt) 
    {
        sockaddr_in a;
        hostent *ent;

        a.sin_family = AF_INET;
        a.sin_port = htons(pt);

        if ((ent = gethostbyname(ip.c_str())) != NULL) 
        {
            memcpy(&a.sin_addr.s_addr, ent->h_addr, ent->h_length);
            addr = string(static_cast<const char *>(static_cast<const void *>(&a)), sizeof a);
        }
    }

public:
    NetAddress() {}

    NetAddress(port pt) 
    {
        sockaddr_in a;
        a.sin_family = AF_INET;
        a.sin_port = htons(pt);
        a.sin_addr.s_addr = INADDR_ANY;
        addr = string(static_cast<const char *>(static_cast<const void *>(&a)), sizeof a);
    }

    NetAddress(string ip, port pt = 0) 
    {
        init(ip, pt);
    }

    NetAddress(string ip, string pt) 
    {
        init(ip, atoi(pt.c_str()));
    }

    NetAddress(const Address& other) 
    {
    if (other && other.data()->sa_family == AF_INET)
        addr = other.data_as_string();
    }

    string get_host() const 
    {
        if (!*this) return string();
        return inet_ntoa(inetAddr()->sin_addr);
    }

    string get_hostname() const 
    {
        if (!*this) return string();

        struct hostent* ent =
            gethostbyaddr(static_cast<const char *>(static_cast<const void *>\
            		(&inetAddr()->sin_addr.s_addr)), sizeof(in_addr_t), AF_INET);

        if (ent)
            return string(ent->h_name);

        return get_host();
    }

    port get_port() const 
    {
        if (!*this) 
        	return 0;

        return ntohs(inetAddr()->sin_port);
    }

    string as_string() const 
    {
        string out = get_host();

        if (get_port()) 
        {
            out += ":";
            out += toString(get_port());
        }
        
        return out;
    }

    static string local_hostname() 
    {
        char name[128];
        if (gethostname(name, sizeof name)) 
        	return "";
        
        return name;
    }
    
};
#endif