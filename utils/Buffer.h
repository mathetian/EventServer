#ifndef _BUFFER_H
#define _BUFFER_H

#include <string>
using std::string;

#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "Atom.h"
#include "Log.h"

class ConstBuffer
{
protected:
    char *dat;
    unsigned int len;

public:
    ConstBuffer() : dat(0), len(0) {}

    ConstBuffer(const void *dat, unsigned int len) :
        dat(static_cast<char*>(const_cast<void*>(dat))), len(len) { }

    ConstBuffer(const string& s) :
        dat(const_cast<char*>(s.data())), len(s.length()) {}

    unsigned int length() const
    {
        return len;
    }

    operator const void *() const
    {
        return dat;
    }

    operator string() const
    {
        return string(dat, len);
    }
};

class InnerBuffer : public ConstBuffer
{
protected:
    unsigned int maxlen;

public:
    InnerBuffer() : ConstBuffer() {}

    InnerBuffer(const void *dat, unsigned int len) : ConstBuffer(dat, len), maxlen(len) {}

    InnerBuffer(const void *dat, unsigned int len, unsigned int maxlen) :
        ConstBuffer(dat, len), maxlen(maxlen) { }

    char *data()
    {
        return dat;
    }

    void set_length(unsigned int len)
    {
        this->len = len;
    }

    unsigned int max_length() const
    {
        return maxlen;
    }
};

class Buffer : public InnerBuffer
{
    Atomic *ref;

private:
    void acquire()
    {
        if (ref) ++*ref;
    }
    void release()
    {
        if (ref && *(ref--) == 0)
        {
            DEBUG << "release finally" ;
            delete[] dat;
            delete ref;
        }
    }

public:
    Buffer() : InnerBuffer(), ref(0)
    {
        acquire();
    }

    Buffer(unsigned int maxlen) : InnerBuffer(new char[len + 1], 0, len + 1), ref(new Atomic(0))
    {
        acquire();
    }

    Buffer(const Buffer &other) : InnerBuffer(other.dat, other.len, other.maxlen), ref(other.ref)
    {
        acquire();
    }

    Buffer(const char* str) : InnerBuffer(str, strlen(str), strlen(str) + 1), ref(new Atomic(0))
    {
        acquire();
    }

    Buffer& operator = (const Buffer &other)
    {
        release();

        dat = other.dat; len = other.len;
        ref = other.ref; maxlen = other.maxlen;
        acquire(); return *this;
    }

    ~Buffer()
    {
        release();
    }
};

#endif