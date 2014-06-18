// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _ATOMIC_H
#define _ATOMIC_H

#include "Header.h"


/**
** Atom counters
** Atomicity is a guarantee of isolation from concurrent processes.
** Additionally, atomic operations commonly have a succeed-or-fail definition
** the system behaves as if each operation occurred instantly
**/

namespace utils
{

static inline int  __exchange_and_add(volatile int *__mem, int __val)
{
    register int __result;
    __asm__ __volatile__ ("lock; xaddl %0,%2"
                          : "=r" (__result)
                          : "0" (__val), "m" (*__mem)
                          : "memory");
    return __result;
}

/**
** An atomic integer class.  Useful for threadsafe smart pointers
** (and probably other stuff too).
**
** Currently implemented in terms of the __exchange_and_add and
** __atomic_add functions in libstdc++.
**/

class Atomic
{
    int val;

public:
    /// Constructs an atomic integer with a given initial value.
    Atomic(int val = 0) : val(val) {}

    /// Atomically increases the value of the integer and returns its
    /// old value.
    inline int exchange_and_add(int addend)
    {
        return __exchange_and_add(&val, addend);
    }

    /// Atomically increases the value of the integer.
    inline void add(int addend)
    {
        __sync_add_and_fetch(&val, addend);
    }

    /// Atomically increases the value of the integer
    /// and return the new value
    inline int addAndGet(int addend)
    {
        return __sync_add_and_fetch(&val, addend);
    }

    /// Atomically increases the value of the integer.
    inline void operator += (int addend)
    {
        add(addend);
    }

    /// Atomically decreases the value of the integer.
    inline void operator -= (int addend)
    {
        add(-addend);
    }

    /// Atomically increments the value of the integer.
    inline void operator ++ ()
    {
        add(1);
    }

    /// Atomically decrements the value of the integer.
    inline void operator -- ()
    {
        add(-1);
    }

    /// Atomically increments the value of the integer and returns its
    /// old value.
    inline int operator ++ (int)
    {
        return exchange_and_add(1);
    }

    /// Atomically decrements the value of the integer and returns its
    /// old value.
    inline int operator -- (int)
    {
        return exchange_and_add(-1);
    }

    /// Returns the value of the integer.
    operator int() const
    {
        return val;
    }
};

};

#endif
