#ifndef _BIND_H
#define _BIND_H

#include <set>
using namespace std;

#include <assert.h>
#include <stdio.h>

template<typename R>
class Callback{
  private:
    struct func 
    {
        virtual R operator () () = 0;
        virtual ~func() {}
    };

    func *m_fn;
  public:

    R operator() () const 
    { 
        assert(m_fn); 
        return (*m_fn)(); 
    }

    template<typename O>
    struct bindfunc : public func 
    {
    	typedef R(O::*fp_t)(); fp_t fp; O& o;
        bindfunc(O& o, fp_t fp) : o(o), fp(fp) { assert(&o); assert(fp); }
        R operator() () { return (o.*fp)(); }
    };

    template<typename O>
    static func* init( O& o, R(O::*fp)() ) 
    {
        return new bindfunc<O>(o, fp);
    }

    template<typename O>
    Callback( O& o, R(O::*fp)() ) : m_fn( init(o, fp) ) {}

    Callback() : m_fn(NULL) { }
    
    ~Callback()
    {
        if(m_fn) delete m_fn;
    }
};

#endif 