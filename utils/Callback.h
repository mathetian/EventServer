// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _BIND_H
#define _BIND_H

#include "Header.h"

namespace utils
{

/**
 * Generic callbacks.
 *
 * A callback<R, T1, T2...> is a function that you can call with
 * arguments T1, T2, ... and obtain a result of type R.  (R may
 * be void, and there can be zero or more arguments.)  For instance:
 *
 * <ul>
 * <li>callback<void> is a function which takes no arguments and returns no result.
 * <li>callback<string, int> is a function which takes an int as a input and returns a string.
 * <li>callback<string, int, vector<int> > is a function which takes an int and a vector<int>
 * as input and returns a string.
 * </ul>
 *
 * You can invoke a callback just as you'd invoke a function:
 *
 * callback<string, int, bool> my_callback = ...;
 * string s = my_callback(3, true);
 *
 * To create a callback on a function, pass a pointer to the
 * function as the first
 * argument to the callback constructor:
 *
 * de
 * string my_function(int a, bool b);
 * callback<string, int, bool> my_callback(&my_function);
 *   ...
 *
 * // later
 * string s = my_callback(3, true);
 * ndcode
 *
 * You can also "bind" arguments to your function.  They are stored
 * in the callback object and are passed to your function when it is
 * invoked later:
 *
 * de
 * string my_function(int a, bool b);
 * callback<string, bool> my_callback(&my_function, 3);
 * // N.B.: no "int" in type, since we've already specified the int
 * // in the callback constructor!
 *   ...
 *
 * // later
 * string s = my_callback(true);
 * ndcode
 *
 * You may also create callbacks from method pointers to objects; just pass a
 * pointer or reference to the object as the first argument to the constructor, and the method
 * pointer second.  (Note that create a method pointer in ISO C++, you
 * must use the syntax &ClassName::method_name; compilers are not
 * supposed to let you simply say &method_name within the class.)
 *
 * de
 * class MyFooClass {
 *     int some_instance_variable;
 *
 *   public:
 *     string bar(int a, bool b);
 * };
 *
 * MyFooClass &foo;
 * callback<string, int, bool> my_callback_1(foo, &MyFooClass::bar);
 * callback<string, bool> my_callback(foo, &MyFooClass::bar, 3);
 *   ...
 *
 * // later
 * string s = my_callback(3, true);
 * string t = my_callback(true);
 * ndcode
 **/

template<typename R>
class Callback
{
private:
    struct func
    {
        virtual R operator () () = 0;
        virtual ~func() {}
        virtual bool stat() = 0;
    };

    func *m_fn;

public:
    R operator() () const
    {
        assert(m_fn);
        return (*m_fn)();
    }

    operator bool()
    {
        return m_fn->stat();
    }

    template<typename O>
    struct bindfunc : public func
    {
        typedef R(O::*fp_t)();
        fp_t fp;
        O* op;

        bindfunc(O* op, fp_t fp) : op(op), fp(fp)
        {
            assert(op);
            assert(fp);
        }

        R operator() ()
        {
            assert(op);
            return (op->*fp)();
        }

        bool stat()
        {
            if(op == NULL)
                return false;
            return true;
        }
    };

    template<typename O>
    static func* init( O* op, R(O::*fp)() )
    {
        return new bindfunc<O>(op, fp);
    }

    template<typename O>
    Callback( O* op, R(O::*fp)() ) : m_fn( init(op, fp) ) {}

    Callback() : m_fn(NULL) { }
};

};

#endif