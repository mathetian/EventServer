// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _STATUS_H
#define _STATUS_H

#include "C.h"

namespace utils
{


/**
** status, the tuple of `flag` and `strerr`
**/
class Status
{
    bool ok;
    string err;

public:
    /// Constructor.  Uses a default success or failure string if err is empty.
    Status(bool ok, string err = string()) : ok(ok), err(err)
    { }

    /// Constructor for failure state.
    Status(string err) : ok(false), err(err)
    { }

    /// Constructor for failure state.
    Status(const char *err) : ok(false), err(err)
    { }

public:
    /// Return the internal flag
    operator bool() const
    {
        return ok;
    }

    /// Returns the status as a human-readble string.
    string as_string() const
    {
        if(ok == true) return "True";
        else return "False :" + err;
    }

public:
    /// Returns a "good" status object.
    static Status good()
    {
        return true;
    }

    /// Returns a "bad" status object.
    static Status bad(string err = string())
    {
        return Status(false, err);
    }

    /// Returns a "bad" status object using the system error information
    static Status syserr(int errcode, string err = string())
    {
        if (err.empty())
            return Status(false, strerror(errcode));
        else
            return Status(false, err + ": " + strerror(errcode));
    }

    /// Returns a "bad" status object using the system error information
    static Status syserr(string err = string())
    {
        return syserr(errno, err);
    }
};

TO_STRING(utils::Status);


};

#endif