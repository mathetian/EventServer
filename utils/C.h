// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _C_H
#define _C_H

#include "Header.h"

/**
** C.h provides some C-like helper functions
**
** It mainly provides `to_string` and `to_escaped_string`
** `to_string` is used to convert any object into human-readable string
** `to_escape_string` is used to convert string to escape_string(\t -> \\t).
**/
namespace utils
{

template<class T>
inline string to_string(const T* t)
{
    char buf[64];
    sprintf(buf, "&(%p)", t);
    return buf;
}

inline string to_string(const char* ch)
{
    return string(ch);
}

inline string to_string(const string& s)
{
    return s;
}

inline string to_string(const void* t)
{
    char buf[64];
    sprintf(buf, "&%p", t);
    return buf;
}

inline string to_string(const bool& i)
{
    static string t = "true";
    static string f = "false";
    return i ? t : f;
}

#define SCALAR_STRING(T) \
inline string to_string(const T& t) { ostringstream o; o << t; return o.str(); }

SCALAR_STRING(char);
SCALAR_STRING(int);
SCALAR_STRING(uint32_t);
SCALAR_STRING(uint64_t);

#define TO_STRING(T) \
    inline string to_string(const T& t) { return t.as_string(); }

inline string to_string(const float& t)
{
    ostringstream o;
    o << t;
    return o.str();
}

inline string to_string(const double& t)
{
    ostringstream o;
    o << t;
    return o.str();
}

inline string to_escaped_string(const void *data, int length)
{
    const unsigned char *cdata = static_cast<const unsigned char *>(data);
    string out;
    while (length--)
    {
        unsigned char ch = *cdata++;
        switch (ch)
        {
        case '\"':
        case '\\':
            out += '\\';
            out += ch;
            break;

        case '\n':
            out += "\\n";
            break;

        case '\r':
            out += "\\r";
            break;

        case '\t':
            out += "\\t";
            break;

        default:

            if (ch >= ' ' && ch <= '~')
                out += char(ch);
            else
            {
                out += '\\';
                if (ch < 8)
                {
                    out += '0' + ch;
                }
                else
                {
                    out += "x";
                    out += "0123456789ABCDEF"[ch >> 4];
                    out += "0123456789ABCDEF"[ch & 0xF];
                }
            }
        }
    }
    return out;
}

inline bool is_int(const string & s)
{
    if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

    char * p ;
    strtol(s.c_str(), &p, 10) ;

    return (*p == 0) ;
}

inline int to_int(string str)
{
    return atoi( str.c_str() );
}

};

#endif