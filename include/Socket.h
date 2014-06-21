// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _SOCKET_H
#define _SOCKET_H

#include "Log.h"
using namespace utils;

#include "Address.h"

namespace sealedserver
{

/**
** Close Method
**
** CLSSIG, by signal
** CLSEOF, by EOF
** CLSERR, by event(Close Event)
** CLSMAN, by manual
**/
enum ClsMtd {CLSSIG = 0, CLSEOF, CLSERR, CLSMAN};

/**
** A network socket descriptor.  This class will be expanded
** to include socket-specific I/O methods.
**/
class Socket
{
public:
    /// Default constructor
    Socket(int fd = -1);

    /// Constructor
    ///
    /// @param family, protocal family
    /// @param type  , protocal type
    Socket(int family, int type);

public:
    /// Returns the file descriptor.
    int fd() const;

    /// Bind and Listen(for server)
    ///
    /// @param paddr, the address of localhost(for server), without any check
    bool bindListen(const Address *paddr);

    /// Connect to the server(for client)
    /// Attempts to connect the socket.
    ///
    /// @param paddr, the address of server, without any check
    bool connect(const Address *paddr);

public:
    /// Attempts to bind the socket.
    bool bind(const Address *paddr);

    /// Listens on the socket.
    bool listen(int backlog = 5);

    /// Attempts to accept a connection on the socket.
    Socket accept(Address *pa);

public:
    /// Close the file descriptor
    void close();

    /// Attempts to read bytes into a buffer. 
    ///
    /// @return the number of bytes actually read
    int read(void *buf, uint32_t count);

    /// Attempts to write bytes from a data buffer.
    ///
    /// @return the number of bytes actually written
    int write(const void *buf, int count);

    /// Returns the peer name of the socket, if any.
    NetAddress getpeername();

    /// Returns the local name of the socket, if any.
    NetAddress getsockname();

    /// Create a pipe of sockets
    static pair<Socket, Socket> pipe();

private:
    /// Sets a handle into blocking or non-blocking mode.
    ///
    /// @return true if successful.
    bool setUnblocking();

private:
    /// file descriptor
    int    m_fd;
};

/// A TCP socket descriptor. 
class TCPSocket : public Socket
{
public:
    /// Constructor
    TCPSocket() : Socket() { }
    
    /// Constructor, passed by a address
    /// Try to instance the socket(by bind and listen)
    TCPSocket(Address *paddr) : Socket(AF_INET, SOCK_STREAM)
    {
        bindListen(paddr);
    }
};

};

#endif