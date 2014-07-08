// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _SLICE_H
#define _SLICE_H

#include "C.h"

namespace utils
{

class Slice
{
protected:
    char *dat;
    unsigned int len;

public:

    Slice() : dat(0), len(0) {}

    Slice(const void *dat, unsigned int len) :
        dat(static_cast<char*>(const_cast<void*>(dat))), len(len) {}

    Slice(const string& s) : dat(const_cast<char*>(s.data())), len(s.length()) {}

    const char *data() const
    {
        return dat;
    }

    const char *str()
    {
        return dat;
    }

    uint32_t length() const
    {
        return len;
    }

    /**Equal to if(buf) { }**/
    operator void *() const
    {
        return dat;
    }

    operator string() const
    {
        return string(dat);
    }

    string as_string() const
    {
        return dat ? "\"" + to_escaped_string(dat, len) + "\"" : "\"\"";
    }

};

TO_STRING(Slice);

};


#endif