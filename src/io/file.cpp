#include "file.hpp"

#include <stdio.h>
#include <sys/stat.h>

bool File::exists(String path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0;
}

File *File::findInPath(String name, String path) {
    String subpath;

    int lastindex = 0;
    int nextindex = 0;

    while(nextindex < path.length()) {
        lastindex = nextindex;
        nextindex = path.indexOf(';', nextindex);
        subpath = path.substring(lastindex + 1, nextindex - lastindex - 1);
        subpath.append('/');
        subpath.append(name);
        if(File::exists(subpath)) {
            return new File(subpath);
        }
    }

    return NULL;
}

File::File(const String &_filename) {
    filename = _filename;
    fp = fopen(filename.c_str(), "rb");
}

File::~File() {
    fclose(fp);
}

String File::getPath() {
    int slash = filename.lastIndexOf('/');

    if(slash > 0) {
        return filename.substring(0, slash);
    }

    return filename;
}

String File::getName() const {
    int slash = filename.lastIndexOf('/');
    return filename.substring(slash+1);
}

String File::getBasename() {
    int slash = filename.lastIndexOf('/');
    int dot = filename.lastIndexOf('.');

    if(dot > slash) {
        return filename.substring(slash+1, dot - slash);
    }

    return filename.substring(slash+1);
}

String File::getExtension() {
    int dot = filename.lastIndexOf('.');
    return filename.substring(dot+1);
}

long File::getSize() {
    if(size < 0) {
        long t = ftell(fp);
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, t, SEEK_SET);
    }
    return size;
}

int File::seek(long sval) {
    return fseek(fp, sval, SEEK_CUR);
}

int File::set(long sval) {
    return fseek(fp, sval, SEEK_SET);
}

int File::rset(long sval) {
    return fseek(fp, sval, SEEK_END);
}

long File::tell() {
    return ftell(fp);
}

int File::get() {
    return fgetc(fp);
}

int File::peek(int ahead) {
    int c = -1;

    if(ahead) {
        fseek(fp, ahead, SEEK_CUR);
        c = fgetc(fp);
        fseek(fp, -(ahead + 1), SEEK_CUR);
    } else {
        c = fgetc(fp);
        ungetc(c, fp);
    }

    return c;
}

long File::read(void *buf, long sz, long nmemb) {
    return fread(buf, sz, nmemb, fp);
}

bool File::eof() {
    return feof(fp);
}
