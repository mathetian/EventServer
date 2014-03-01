#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <sstream>
using namespace std;

#include <stdint.h>

inline string to_string(uint32_t value)
{
	ostringstream o; o << value; 
	return o.str();
}

// inline string to_string(bool flag)
// {
//     if(flag == true)
//         return "True";
//     else 
//         return "False";
// }

inline string to_escaped_string(const void *data, int length) 
{
    const unsigned char *cdata = static_cast<const unsigned char *>(data);
    string out;
    while (length--) {
        unsigned char ch = *cdata++;
        switch (ch) {
          case '\"': case '\\':
            out += '\\'; out += ch;
            break;

          case '\n':
            out += "\\n";
            break;

          case '\r':
            out += "\\r";
            break;

          case '\t':
            out += "\\t";
            break;

          default:

            if (ch >= ' ' && ch <= '~')
                out += char(ch);
            else 
            {
                out += '\\';
                if (ch < 8) {
                    out += '0' + ch;
                } else {
                    out += "x";
                    out += "0123456789ABCDEF"[ch >> 4];
                    out += "0123456789ABCDEF"[ch & 0xF];
                }
            }
        }
    }
    return out;	
}

#endif