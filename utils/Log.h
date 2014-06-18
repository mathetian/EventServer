// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _LOG_H
#define _LOG_H

#include "C.h"

/**
** Log.h: The logging framework.
*/

namespace utils
{

/**
** An internal class used to implement the logging framework.
**/

class Log
{
public:
    static ostream* m_out;

    static Log m_log;
    static int m_level;

    class LogMsg;
public:
    enum
    {
        fatal = 0,
        error = 1,
        warn  = 2,
        info  = 3,
        debug = 4
    };

    Log(int level = warn, ostream *out = &cout)
    {
        m_level = level;
        m_out = out;
    }

    static Log& get()
    {
        return m_log;
    }

    static int getLevel()
    {
        return m_level;
    }
    bool operator ^ (const LogMsg& msg);
};

/**
** An internal class used to implement the logging framework.  (See
** the log class for information on the debugging architecture.)
** Shouldn't use it directly
*/

class Log::LogMsg
{
    const char *m_file;
    int         m_line;
    int         m_level;

    string str;

public:
    static const char *level2string(int level)
    {
        switch(level)
        {
        case Log::fatal:
            return "FATAL";
            break;
        case Log::error:
            return "ERROR";
            break;
        case Log::warn:
            return "WARN";
            break;
        case Log::info:
            return "INFO";
            break;
        case Log::debug:
            return "DEBUG";
            break;
        }

        return "";
    }

public:
    LogMsg(const char *file, int line, const char *func, int level) :
        m_file(file), m_line(line), m_level(level)
    { }

    template<class T>
    LogMsg& operator << (const T& t)
    {
        str += to_string(t);
        return *this;
    }
    LogMsg& operator << (const string& s)
    {
        str += s;
        return *this;
    }
    LogMsg& operator << (const char* s)
    {
        str += s;
        return *this;
    }
    void write(ostream &o) const
    {
        string out;

        const char *slash = strrchr(m_file, '/');
        const char *file_str = slash ? slash + 1 : m_file;

        out += level2string(m_level);
        if(m_level == Log::warn || m_level == Log::info)
            out += " ";
        out += " [";
        out += file_str;
        out += ":";
        ostringstream o1;
        o1 << m_line;
        out += o1.str();
        out += "] ";

        out += str + "\n";
        o << out;
    }
};

/**
** <p>You shouldn't use this class directly.  Instead, use it like this:
**
** \code
** DEBUG << "foo is " << foo
** INFO << "Accepted connection from " << addr;
** WARN << "...";
** ERROR << "...";
** FATAL << "Assertion failed";  // Also kills program
** CORE << "Assertion failed";   // Kills program, leaving core dump
** \endcode
** You can use feed just about anything, type-safely, as arguments to
** <code>&lt;&lt;</code> (consider it varargs, C++ style).
**
** This prints messages like the following to standard error:
**
** \code
** debug [file.c:30] Foo is bar
** info  [file.c:31] Accepted connection from 18.7.16.73:9133
** warn  [file.c:32] ...
** error [file.c:33] ...
** fatal [file.c:34] Assertion failed
** core! [file.c:35] Assertion failed
** \endcode
**
**/
#define LOG(level) (Log::getLevel() >= Log::level) && Log::get() ^ Log::LogMsg(__FILE__, __LINE__, __PRETTY_FUNCTION__, Log::level)

#define DEBUG    LOG(debug)
#define INFO     LOG(info)
#define WARN     LOG(warn)
#define ERROR    LOG(error)
#define FATAL    LOG(fatal)

inline bool Log::operator ^ (const Log::LogMsg &msg)
{
    msg.write(*m_out);
}

};
#endif