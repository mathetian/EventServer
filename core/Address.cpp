// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Address.h"

namespace sealedserver
{

Address::Address() { }

Address::Address(const void *addr, socklen_t len) :
    m_addr(static_cast<const char*>(addr), len) { }

const sockaddr *Address::data() const
{
    return static_cast<const sockaddr *>\
           (static_cast<const void *>(m_addr.data()));
}

socklen_t Address::length() const
{
    return m_addr.length();
}

void   Address::setAddr(const void *addr, socklen_t len)
{
    m_addr = string(static_cast<const char *>(addr), len);
}

NetAddress::NetAddress() { }

NetAddress::NetAddress(port pt)
{
    sockaddr_in a;
    a.sin_family = AF_INET;
    a.sin_port   = htons(pt);
    a.sin_addr.s_addr = INADDR_ANY;

    m_addr = string(static_cast<const char *>(static_cast<const void *>(&a)), sizeof a);
}

NetAddress::NetAddress(string ip, port pt)
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

NetAddress::NetAddress(const void *addr, socklen_t len) : Address(addr, len) { }

const sockaddr_in* NetAddress::inetAddr() const
{
    return static_cast<const sockaddr_in *>\
           (static_cast<const void *>(data()));
}

string NetAddress::IP() const
{
    return inet_ntoa(inetAddr()->sin_addr);
}

uint32_t NetAddress::Port() const
{
    return ntohs(inetAddr()->sin_port);
}

// string NetAddress::as_string() const
// {
//     string out = IP();

//     out += ":" + to_string(Port());

//     return out;
// }

};
