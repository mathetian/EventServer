#ifndef _STATUS_H
#define _STATUS_H

#include <string>
using std::string;

#include <errno.h>
#include <string.h>

#include <iostream>
using namespace std;

#include "../utils/Utils.h"

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

    operator bool()
    {
        return ok;
    }

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
        else return "False";
    }
};

TO_STRING(Status);

#endif