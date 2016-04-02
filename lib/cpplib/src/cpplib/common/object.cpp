#include "object.hpp"

Object::Object() {
    refcount = 1;
}

Object::~Object() {
}

int Object::retain() {
    return ++refcount;
}

int Object::release() {
    int count = --refcount;

    if(!count) {
        delete this;
        return 0;
    }

    return count;
}
