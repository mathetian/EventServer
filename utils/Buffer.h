#ifndef _BUFFER_H
#define _BUFFER_H

#include <string>
using std::string;

#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "Atomic.h"

namespace utils
{

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

    void zeros()
    {
        memset(dat,0,maxlen);
    }
};

class Buffer : public InnerBuffer
{
    Atomic *ref;
    int self_alloc;
private:
    void acquire()
    {
        if (ref) ++*ref;
    }
    void release()
    {
        if(self_alloc == 0) return;
        if (ref && (ref->addAndGet(-1)) == 0)
        {
            delete[] dat;
            delete ref;
            ref = NULL;
        }
    }

public:
    Buffer() : InnerBuffer(), ref(0)
    {
        acquire();
        self_alloc = 0;
    }

    Buffer(unsigned int maxlen) : InnerBuffer(new char[maxlen + 1], 0, maxlen + 1), ref(new Atomic(0))
    {
        zeros();
        acquire();
        self_alloc = 1;
    }

    Buffer(const Buffer &other) : InnerBuffer(other.dat, other.len, other.maxlen), ref(other.ref)
    {
        acquire();
        self_alloc = other.self_alloc;
    }

    Buffer(const char* str, int self_alloc = 0) : InnerBuffer(str, strlen(str), strlen(str) + 1), ref(new Atomic(0))
    {
        acquire();
        this->self_alloc = self_alloc;
        if(self_alloc == 1) zeros();
    }

    Buffer(string str) : InnerBuffer(str.data(), str.size(), str.size() + 1), ref(new Atomic(0))
    {
        acquire();
        this->self_alloc = 0;
    }

    Buffer& operator = (const Buffer &other)
    {
        release();

        dat = other.dat;
        len = other.len;
        ref = other.ref;
        maxlen = other.maxlen;
        acquire();
        self_alloc = other.self_alloc;

        return *this;
    }

    ~Buffer()
    {
        release();
    }
};

};
#endif