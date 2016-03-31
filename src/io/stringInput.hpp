#ifndef _STRINGINPUT_HPP
#define _STRINGINPUT_HPP

#include "io/string.hpp"
#include "io/input.hpp"

class StringInput : public Input {
	long offset;
	String str;

	public:
	StringInput(const String &_str);
    virtual long getSize();
    virtual int seek(long sval);
    virtual int set(long sval);
    virtual int rset(long sval);
    virtual long tell();
    virtual int get();
    virtual int peek(int ahead = 0);
    virtual long read(void *buf, long sz, long nmemb);
    virtual bool eof();
    virtual String getName() const;
};

#endif
