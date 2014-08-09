// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _BUFFER_H
#define _BUFFER_H

#include "Atomic.h"

namespace utils
{

/**
** An IMutableBuff data buffer.  Equivalent to a pair<const void*,
** size_t>: this is just a convenient way of passing a reference to
** an IMutableBuff data buffer.
**/
class IMutableBuff
{
protected:
    char *dat;
    unsigned int len;

public:
    IMutableBuff() : dat(0), len(0) {}

    IMutableBuff(const void *dat, unsigned int len) :
        dat(static_cast<char*>(const_cast<void*>(dat))), len(len) { }

    IMutableBuff(const string& s) :
        dat(const_cast<char*>(s.data())), len(s.length()) {}

    unsigned int length() const
    {
        return len;
    }

    unsigned int size() const
    {
        return length();
    }

    operator const void *() const
    {
        return dat;
    }

    operator string() const
    {
        return string(dat, len);
    }

    void set_data(const void *dat)
    {
        this -> dat = static_cast<char*>(const_cast<void*>(dat));
    }

    char operator[](int index)
    {
        assert(index < len);
        return dat[index];
    }
};

/**
** A mutable data buffer.  Basically equivalent to a pair<void *,
** size_t>: this is just a convenient way of passing a reference to a
** mutable data buffer.
**
** A DynamicBuff actually contains both a "current length" and a "maximum
** length" for the data buffer; for instance, one may allocate <i>n</i>
** bytes of storage, and the constructor <code>databuf(mem, 0, <i>n</i>)</code>
** to reflect the fact that there is currently no data in the buffer
** but that it has space for a kilobyte of data.  Input routines such as
** iohandle::read generally read up to <code>max_length()</code> bytes
** and use <code>set_length()</code> to reflect the number of bytes
** actually nead.
**/

class MutableBuff : public IMutableBuff
{
protected:
    unsigned int maxlen;

public:
    MutableBuff() : IMutableBuff() {}

    MutableBuff(const void *dat, unsigned int len) : IMutableBuff(dat, len), maxlen(len) {}

    MutableBuff(const void *dat, unsigned int len, unsigned int maxlen) :
        IMutableBuff(dat, len), maxlen(maxlen) { }

    char *data() const
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

/**
** A dynamically allocated, reference-counted mutable data buffer.
** Constructing a Buffer object allocates a fixed amount of memory,
** which is automatically deallocated when the last Buffer pointing
** to that memory is deleted.
**
** Note that a Buffer may be cast to a databuf or a constbuf, but
** not vice versa.  Also, copying a Buffer to a IMutableBuff or MutableBuff
** does <i>not</i> create another reference to the underlying data.
**/

class Buffer : public MutableBuff
{
    Atomic *ref;
    bool self_alloc;

private:
    void acquire()
    {
        if (ref) ++*ref;
    }

    void release()
    {
        if(self_alloc == false) return;
        if (ref && (ref->addAndGet(-1)) == 0)
        {
            delete[] dat;
            delete ref;
            ref = NULL;
        }
    }

public:
    /// Null constructor.
    Buffer() : MutableBuff(), ref(0), self_alloc(false)
    {
        acquire();
    }

    /// Dynamically allocates a data buffer whose current length is
    /// 0 bytes but whose maximum length is <i>maxlen</i> bytes.
    /// The underlying storage will be deallocate when the last
    /// Buffer referring to that storage is destructed.
    Buffer(unsigned int maxlen) : MutableBuff(new char[maxlen], 0, maxlen), \
        ref(new Atomic(0)), self_alloc(true)
    {
        acquire();
        zeros();
    }

    Buffer(const char* str, bool self_alloc = false) : MutableBuff(str, strlen(str), strlen(str)), \
        ref(new Atomic(0)), self_alloc(self_alloc)
    {
        if(self_alloc == false)
        {
            self_alloc = true;
            char *dat1 = new char[strlen(str) + 1];
            memset(dat1, 0, strlen(str) + 1);
            memcpy(dat1, str, strlen(str));

            set_data(dat1);
        }

        acquire();
    }

    Buffer(const string &str) : MutableBuff(str.data(), str.size(), str.size()), ref(new Atomic(0)), self_alloc(0)
    {
        if(self_alloc == false)
        {
            self_alloc = true;
            char *dat1 = new char[str.size() + 1];
            memset(dat1, 0, str.size() + 1);
            memcpy(dat1, str.c_str(), str.size());

            set_data(dat1);
        }

        acquire();
    }

    /// Copies a Buffer, making another reference to <i>other</i>'s
    /// underlying storage.
    Buffer(const Buffer &other) : MutableBuff(other.dat, other.len, other.maxlen), \
        ref(other.ref), self_alloc(other.self_alloc)
    {
        acquire();
    }

    /// Copies a dynbuf, making another reference to <i>other</i>'s
    /// underlying storage.
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

    /// Deletes the underlying storage of this dynbuf if it is the
    /// last existing copy.
    ~Buffer()
    {
        release();
    }
};

};
#endif