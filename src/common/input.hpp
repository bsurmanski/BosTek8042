#ifndef _INPUT_HPP
#define _INPUT_HPP

#include "common/string.hpp"

class Input {
    public:
	virtual ~Input() {}
    virtual long getSize() = 0;
    virtual int seek(long sval) = 0;
    virtual int set(long sval) = 0;
    virtual int rset(long sval) = 0;
    virtual long tell() = 0;
    virtual int get() = 0;
    virtual int peek(int ahead = 0) = 0;
    virtual long read(void *buf, long sz, long nmemb) = 0;
    virtual bool eof() = 0;

    virtual String getName() const = 0;
};

#endif
