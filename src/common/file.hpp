#ifndef _FILE_HPP
#define _FILE_HPP

#include <stdio.h>

#include "input.hpp"

class File : public Input {
    String filename;
    FILE *fp;
    long size;

    public:
    static bool exists(String filename);
    static File *findInPath(String name, String path);

    File(const String &_filename);
	~File();
    String getPath();
    virtual String getName() const;
    String getBasename();
    String getExtension();

    String toString();

    long getSize();
    int seek(long sval);
    int set(long sval);
    int rset(long sval);
    long tell();
    int get();
    int peek(int ahead = 0);
    long read(void *buf, long sz, long nmemb);
    bool eof();
};

#endif
