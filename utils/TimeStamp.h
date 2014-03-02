#ifndef _TIME_STAMP_H
#define _TIME_STAMP_H

#include "Utils.h"

#include <sys/time.h>

class TimeStamp {
  private:
    uint64_t m_val;

  public:
    TimeStamp() : m_val(0) {}

    TimeStamp(uint64_t val) : m_val(val) 
    { }

    static TimeStamp now() 
    {
    	struct timeval tv;
    	gettimeofday(&tv, 0);
    	return TimeStamp(tv.tv_sec * 1000000LL + tv.tv_usec);
    }


    uint64_t to_secs() const 
    { 
        return m_val / 1000000LL; 
    }

    uint64_t to_msecs() const 
    { 
        return m_val / 1000LL; 
    }

    uint64_t to_usecs() const 
    { 
        return m_val; 
    }

    timespec to_timespec() const 
    {
	   struct timespec ret;
	   ret.tv_sec  = m_val / 1000000LL;
	   ret.tv_nsec = (m_val % 1000000LL) * 1000;
	   return ret;
    }

    timeval to_timeval() const 
    {
    	struct timeval ret;
    	ret.tv_sec  = m_val / 1000000LL;
    	ret.tv_usec = m_val % 1000000LL;
    	return ret;
    }

    operator const void *() const 
    {
	   return *this != none() ? this : 0;
    }

    TimeStamp operator + (TimeStamp other) const 
    {
	   return TimeStamp(m_val + other.m_val);
    }

    TimeStamp operator - (TimeStamp other) const 
    {
	   return TimeStamp(m_val - other.m_val);
    }

    TimeStamp& operator += (TimeStamp other) 
    {
    	m_val += other.m_val;
    	return *this;
    }

    TimeStamp& operator -= (TimeStamp other) 
    {
	   m_val -= other.m_val;
	   return *this;
    }

    static TimeStamp none() 
    { 
        return TimeStamp(); 
    }

    string as_string() const 
    { 
        return to_string(to_usecs() / 1000000.0); 
    }

    bool operator()(const TimeStamp &tmp)
    {
        return m_val > tmp.m_val ? false : true;
    }

    static TimeStamp usecs(int val)
    {
        return TimeStamp(val);
    }
};

TO_STRING(TimeStamp);

#endif