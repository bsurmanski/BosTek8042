#ifndef _GEOM_BALL_HPP
#define _GEOM_BALL_HPP

#include "vec.hpp"

class Ball {
    float _radius;
    Vec4 _center;

    public:
    Ball(const Vec4 &_center, float radius);
    Ball(float x, float y, float z, float radius);
    void set_center(const Vec4 &_center);
    void set_center(float x, float y, float z);
    void set_radius(float r);
    void scale(float s);
    void move(const Vec4 &dv);
    void move(float x, float y, float z);
    bool collides(const Ball &o);
};

#endif
