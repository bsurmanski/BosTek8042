#ifndef _OBJECT_HPP
#define _OBJECT_HPP

class Object {
    int refcount;
    public:

    Object();
    virtual ~Object();
    int retain();
    int release();
};

#endif
