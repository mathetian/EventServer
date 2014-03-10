#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <sstream>
using namespace std;

#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

#endif