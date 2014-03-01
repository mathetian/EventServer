#ifndef _STATUS_H
#define _STATUS_H

#include <string>
using std::string;

#include <errno.h>
#include <string.h>

#include <iostream>
using namespace std;

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

    operator bool() { return ok; }

    static Status good() { return true; }

    static Status bad(string err = string()) { return Status(false, err); }

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

    string to_string()
    {
        string rs;
        if(ok == true)
            rs += "true";
        else
        {
            rs += "false:";
            rs += err;
        }
        return rs;
    }
};

#endif