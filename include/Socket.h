#ifndef _SOCKET_H
#define _SOCKET_H

#include "Status.h"
#include "Address.h"

#include <vector>
using namespace std;

#include <sys/un.h>
#include <assert.h>

#include <iostream>
using namespace std;

#include "../utils/Log.h"

class Socket 
{
private:
    int    m_fd;
    Status m_stat;

public:  
    typedef enum {
        none = 0,
        nonblocking = 1,
        acceptor = 2
    } Flags;

    static int sys_socket(int domain, int protocal, int type) 
    { 
        return ::socket(domain, protocal, type); 
    }

public:
    Socket(int fd = -1) : m_fd(fd), m_stat(true) { }

    Socket(int family, int type) : m_fd(sys_socket(family, type, 0)), m_stat(true) {}

    Socket(int family, int type, Address addr, Flags f = nonblocking) :
        m_fd(sys_socket(family, type, 0)), m_stat(true)
    {
        // if (f & nonblocking)
        //     set_blocking(false);
        if (addr) 
        {
            if (f & acceptor) 
            {
                int optval = 1;
                setsockopt(get_fd(), SOL_SOCKET, SO_REUSEADDR,
                       &optval, sizeof(optval));

                bind(addr);
                listen();
                if(stat() == false)
                    ERROR << "SOCKET LISTEN ERROR";
                else
                    DEBUG << "LISTEN SUCCESSFULLY";
            } 
            else 
            {
                connect(addr);
            }
        }
    }

    int get_fd() const 
    { 
        assert(m_fd >= 0);
        return m_fd;
    }

private:
    void set_status(const Status& s) 
    { 
        m_stat = s;
    }

    Status bind(const Address& a) 
    {
        if (::bind(get_fd(), a.data(), a.length()) != 0) 
        {
            set_status(Status::syserr("bind"));
        }
        return stat();
    }

    Status listen(int backlog = 5) 
    {
        if (::listen(get_fd(), backlog) != 0) 
        {
            set_status(Status::syserr("listen"));
        }
        return stat();
    }
public:
    /**confused with this function**/
    Socket accept(Address& a) 
    {
        sockaddr addr;
        socklen_t addrlen = sizeof addr;

        int new_fd = ::accept(get_fd(), &addr, &addrlen);
        if (new_fd < 0) 
        {
            set_status(Status::syserr("accept"));
            a = Address();
            return Socket();
        }
        a = Address(&addr, addrlen);

        return Socket(new_fd);
    }
private:
    Status connect(const Address& a) 
    {             
        if (::connect(get_fd(), a.data(), a.length()) != 0) 
        {
            if (errno != EINPROGRESS)
                set_status(Status::syserr("connect"));
        }

        return stat();
    }

    bool set_blocking(bool block) 
    {
        int flags = fcntl(get_fd(), F_GETFL);
        assert(flags >= 0);
        flags = block ? flags & ~O_NONBLOCK : flags | O_NONBLOCK;
        assert(fcntl(get_fd(), F_SETFL, flags) >= 0);
        
        return true;
    }

public:
    NetAddress getpeername() 
    {
        char buf[sizeof(sockaddr_in) + 1];
        socklen_t len = sizeof(buf);

        int ret = ::getpeername(get_fd(), (sockaddr*)&buf, &len);

        if (ret < 0) 
            return NetAddress();
        else if (len == sizeof(buf)) // possible overflow
            return NetAddress();

        return NetAddress(buf, len);
    }

    NetAddress getsockname() 
    {

        char buf[sizeof(sockaddr_in) + 1];
        socklen_t len = sizeof(buf);
        int ret = ::getsockname(get_fd(), (sockaddr*)&buf, &len);

        if (ret < 0)
            return NetAddress();
        else if (len == sizeof(buf)) // possible overflow
            return NetAddress();

        return NetAddress(buf, len);
    }

public:
    int read(void *buf, uint32_t count) 
    {            
        count = ::read(get_fd(), buf, count);

        if(count < 0 && errno == EAGAIN)
        { //log warn 
        }
        else if (count < 0)
            set_status(Status::syserr("Socket::read"));

		return count;
    }

    int readn(void *buf, unsigned int count) 
    {
		int orig_count = count;
		
		while (count) 
		{
		    int bytes = ::read(get_fd(), buf, count);

		    if (bytes < 0 && errno == EAGAIN)
            {
                //log error
            }
			else if(bytes < 0)
			{
				set_status(Status::syserr("iohandle::read_fully"));
				return 0;
			}

		    if (bytes == 0) return 0;

		    assert(bytes <= int(count));

		    buf = (char*)buf + bytes;
		    count -= bytes;
		}
		
		return orig_count;
    }

    int write(const void *buf, int count) 
    {
		count = ::write(get_fd(), buf, count);
        if(count < 0 && errno == EAGAIN)
        {
            //log_warning
        }
        else if(count < 0)
            set_status(Status::syserr("write"));

		return count;
    }

    int writen(const void *buf, unsigned int count) 
    {
		int orig_count = count;
		while (count) 
		{
		    int bytes = ::write(get_fd(), buf, count);

		    if (bytes < 0 && errno == EAGAIN)
            {
                //log warning
            }
			else if(bytes < 0)
			{
				set_status(Status::syserr("writen"));
				return bytes;
			}
		    else if (bytes == 0)  return bytes;

		    assert(bytes <= int(count));
		    buf = static_cast<const char*>(buf) + bytes;
		    count -= bytes;
		}
	   	
		return orig_count;
    }

public:

    Status stat() const 
    {
		assert(get_fd() >= 0);
        return m_stat;
    }

    /**pipe for local**/
    static pair<Socket, Socket> pipe() 
    {
        int fds[2]; ::pipe(fds);
        return pair<Socket, Socket>(Socket(fds[0]), Socket(fds[1]));
    }
    
    operator bool()
    {
        return m_fd >= 0 ? true : false;
    }

};

class TCPSocket : public Socket 
{
  public:
    TCPSocket() : Socket() {}
    TCPSocket(Address addr, Flags f = none) : Socket(AF_INET, SOCK_STREAM, addr, f) { }
    TCPSocket(const Socket& sock) : Socket(sock) {}
};

#endif