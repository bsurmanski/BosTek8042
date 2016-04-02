#include "ball.hpp"

#include <string.h>

Ball::Ball(const Vec4 &center, float radius) : _center(center), _radius(radius) {
}

Ball::Ball(float x, float y, float z, float radius) : _center(x, y, z), _radius(radius) {
}

void Ball::set_center(const Vec4 &center) {
    _center += center;
}

void Ball::set_center(float x, float y, float z) {
    _center.x = x;
    _center.y = y;
    _center.z = z;
}

void Ball::set_radius(float r) {
    _radius = r;
}

void Ball::scale(float s) {
    _radius *= s;
}

void Ball::move(const Vec4 &o) {
    _center += o;
}

void Ball::move(float x, float y, float z) {
    move(Vec4(x, y, z));
}

bool Ball::collides(const Ball &o) {
    float drsq = (_radius + o._radius) * (_radius + o._radius);
    float dvsq = _center.distsq(o._center);
    return drsq > dvsq;
}
