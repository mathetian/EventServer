#ifndef _SLICE_H
#define _SLICE_H

#include "Utils.h"

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
        
    const char *data() const { return dat; }

    uint32_t length() const { return len; }

    /**Equal to if(buf) { }**/
    operator void *() const { return dat; }

    operator string() const 
    { 
    	return string(dat, len); 
    }

    string as_string() const 
    {
		return dat ? "\"" + to_escaped_string(dat, len) + "\"" : "\"\"";
    }
};

#endif