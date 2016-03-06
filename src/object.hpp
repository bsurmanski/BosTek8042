#ifndef _OBJECT_HPP
#define _OBJECT_HPP

class Object {
    int refcount;
    public:
    Object() {
        refcount = 1;
    }

    virtual ~Object() {}

    int retain() {
        refcount++;
    }

    int release() {
        refcount--;
        if(refcount <= 0) {
            delete this;
            return 0;
        }
        return refcount;
    }
};

#endif
