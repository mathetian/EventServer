// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _STATUS_H
#define _STATUS_H

#include "C.h"

namespace sealedServer
{

class Status
{
    bool ok;
    string err;

public:
    Status(bool ok, string err = string()) : ok(ok), err(err)
    { }

    Status(string err) : ok(false), err(err)
    { }

    Status(const char *err) : ok(false), err(err)
    { }

    Status(Status const& status) : ok(status.ok),
        err(status.err) { }

    // operator bool()
    // {
    //     return ok;
    // }

    static Status good()
    {
        return true;
    }

    static Status bad(string err = string())
    {
        return Status(false, err);
    }

    static Status syserr(int errcode, string err = string())
    {
        if (err.empty())
            return Status(false, strerror(errcode));
        else
            return Status(false, err + ": " + strerror(errcode));
    }

    static Status syserr(string err = string())
    {
        return syserr(errno, err);
    }

    string as_string() const
    {
        if(ok == true) return "True";
        else return "False :" + err;
    }

    TO_STRING(sealedServer::Status);
};


};

#endif