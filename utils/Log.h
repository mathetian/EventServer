#ifndef _LOG_H
#define _LOG_H

#include <iostream>
#include <string>
using namespace std;

#include "Utils.h"

#include <string.h>

namespace utils
{

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

    Log(int level = debug, ostream *out = &cout)
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
        out += to_string(m_line);
        out += "] ";

        out += str + "\n";
        o << out;
    }
};

#define LOG(level) (Log::getLevel() >= Log::level) && Log::get() ^ Log::LogMsg(__FILE__, __LINE__, __PRETTY_FUNCTION__, Log::level)

#define DEBUG    LOG(debug)
#define INFO     LOG(info)
#define WARN     LOG(warn)
#define ERROR    LOG(error)
#define FATAL    LOG(fatal)

ostream* Log::m_out;
Log      Log::m_log;
int      Log::m_level;

inline bool Log::operator ^ (const Log::LogMsg &msg)
{
    msg.write(*m_out);
}

}
#endif