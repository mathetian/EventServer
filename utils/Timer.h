// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _TIME_STAMP_H
#define _TIME_STAMP_H

#include "Header.h"

namespace utils
{

/**
** A class encapsulating a duration or a point in time.
**/
class Timer
{
private:
    uint64_t m_val;

public:
    /// Default (null) constructor.
    Timer() : m_val(0) {}

    Timer(uint64_t val) : m_val(val)
    { }

    /// Returns the current time.
    static Timer now()
    {
        struct timeval tv;
        gettimeofday(&tv, 0);
        return Timer(tv.tv_sec * 1000000LL + tv.tv_usec);
    }

    /// Returns the duration in seconds.
    uint64_t to_secs() const
    {
        return m_val / 1000000LL;
    }

    /// Returns the duration in milliseconds.
    uint64_t to_msecs() const
    {
        return m_val / 1000LL;
    }

    /// Returns the duration in microseconds.
    uint64_t to_usecs() const
    {
        return m_val;
    }

    /// Returns the duration as a timespec.
    timespec to_timespec() const
    {
        struct timespec ret;
        ret.tv_sec  = m_val / 1000000LL;
        ret.tv_nsec = (m_val % 1000000LL) * 1000;
        return ret;
    }

    /// Returns the duration as a timeval.
    timeval to_timeval() const
    {
        struct timeval ret;
        ret.tv_sec  = m_val / 1000000LL;
        ret.tv_usec = m_val % 1000000LL;
        return ret;
    }

    /// Returns true unless the time is equal to the special
    /// `0` value.
    operator bool() const
    {
        if(m_val == 0) return false;
        return true;
    }

    /// Returns the sum of two times.
    Timer operator + (Timer other) const
    {
        return Timer(m_val + other.m_val);
    }

    /// Returns the difference of two times.
    Timer operator - (Timer other) const
    {
        return Timer(m_val - other.m_val);
    }

    /// Adds to this time.
    Timer& operator += (Timer other)
    {
        m_val += other.m_val;
        return *this;
    }

    /// Subtract from this time.
    Timer& operator -= (Timer other)
    {
        m_val -= other.m_val;
        return *this;
    }

    /// Returns a special time value meaning "no time at all."
    static Timer none()
    {
        return Timer();
    }

    /// Returns the comaration of Timer
    bool operator()(const Timer &tmp) const
    {
        return m_val > tmp.m_val ? false : true;
    }

    /// Creates a time value corresponding to a particular number
    /// of microseconds.
    static Timer usecs(int val)
    {
        return Timer(val);
    }
};

};

#endif