// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _ADDRESS_H
#define _ADDRESS_H

#include "C.h"
#include "Noncopyable.h"
using namespace utils;

namespace sealedserver
{

/**
** A network address.  Addresses, like STL strings, may be
** passed and copied by value.
**/
class Address
{
public:
    /// Constructs an empty address.
    Address();

    /// Constructs an address from a block of data.
    Address(const void *addr, socklen_t len);

public:
    /// Returns a pointer to the data as a sockaddr.
    const sockaddr *data() const;

    /// Returns the length of the address structure.
    socklen_t       length() const;

    /// Accept will return a address, set it manually
    void   setAddr(const void *addr, socklen_t len);

protected:
    string m_addr;
};

/**
** An IP address and port.
** IPv4 addresses are supported
**/
class NetAddress : public Address
{
private:
    typedef uint32_t port;

public:
    /// Constructs an empty address.
    NetAddress();

    /// Constructs an address from a port(used by server)
    NetAddress(port pt);

    /// Constructs an address from a host name and port.
    /// Convert ip/pt to data to sockaddr_in
    /// later converted to string
    NetAddress(string ip, port pt);

    NetAddress(const void *addr, socklen_t len);

public:
    /// Returns the corresponding ip
    string IP() const;

    /// Returns the corresponding port
    port   Port() const;

    /// Returns a human-readable version of this address.
    string as_string() const
    {
        string out = IP();

        out += ":" + to_string(Port());

        return out;
    }

private:
    void init(string ip, port pt);

    /**
    ** Convert data to sockaddr_in
    **/
    const sockaddr_in *inetAddr() const;
};

TO_STRING(NetAddress);

};

#endif