#include "vec.hpp"

#include "mat.hpp"
#include <math.h>
#include <float.h>
#include <stdio.h>

Vec4::Vec4() : x(0), y(0), z(0), w(0) {
}

Vec4::Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {
}

Vec4 Vec4::createQuat(float angle, const Vec4 &axis) {
    Vec4 ret;
    float scale = sin(angle / 2.0f);
    ret.x = axis.x * scale;
    ret.y = axis.y * scale;
    ret.z = axis.z * scale;
    ret.w = cos(angle / 2.0f);
    return ret;
}

Vec4 &Vec4::operator+=(const Vec4 &o) {
    x += o.x;
    y += o.y;
    z += o.z;
    return *this;
}

Vec4 &Vec4::operator-=(const Vec4 &o) {
    x -= o.x;
    y -= o.y;
    z -= o.z;
    return *this;
}

Vec4 Vec4::operator+(const Vec4 &o) const {
    return Vec4(x + o.x, y + o.y, z + o.z);
}

Vec4 Vec4::operator-(const Vec4 &o) const {
    return Vec4(x - o.x, y - o.y, z - o.z);
}

float &Vec4::operator[](int i) {
    return v[i];
}

float Vec4::operator[](int i) const {
    return v[i];
}


float &Vec4::get(int i) {
    return v[i];
}

float Vec4::get(int i) const {
    return v[i];
}

void Vec4::set(int i, float val) {
    v[i] = val;
}

Vec4 Vec4::operator*(const Mat4 &o) const {
    return this->mul(o);
}

Vec4 Vec4::mul(const Mat4 &o) const {
    Vec4 ret;

    for(int j = 0; j < 4; j++) {
        ret[j] = 0.0f;
        for(int i = 0; i < 4; i++) {
            ret[j] += get(i) * o.get(i, j);
        }
    }

    return ret;
}

Vec4 Vec4::qMul(const Vec4 &o) const {
    return Vec4(
            w * o.x + x * o.w + y * o.z - z - o.y,
            w * o.y + y * o.w + z * o.x - x - o.z,
            w * o.z + z * o.w + x * o.y - y - o.x,
            w * o.w - x * o.x - y * o.y - z - o.z);
}

Vec4 Vec4::qConj() const {
    return Vec4(-x, -y, -z, w);
}

Vec4 Vec4::qRotate(const Vec4 &p) const {
    Vec4 q = normalized();
    Vec4 t = q.qMul(p);
    Vec4 u = t.qMul(q.qConj());
    u.w = 0;
    return u;
}

Mat4 Vec4::qToMat() const {
	float ww = w * w;
	float wx2 = 2.0f * w * x;
	float wy2 = 2.0f * w * y;
	float wz2 = 2.0f * w * z;
	float xx = x * x;
	float xy2 = 2.0f * x * y;
	float xz2 = 2.0f * y * z;
	float yy = y * y;
	float yz2 = 2.0f * y * z;
	float zz = z * z;

	Mat4 ret;
	ret.set(0, 0, ww + xx - yy - zz);
	ret.set(0, 1, xy2 + wz2);
	ret.set(0, 2, xz2 - wy2);
	ret.set(0, 3, 0.0f);

	ret.set(1, 0, xy2 - wz2);
	ret.set(1, 1, ww - xx + yy - zz);
	ret.set(1, 2, yz2 + wx2);
	ret.set(1, 3, 0.0f);

	ret.set(2, 0, xz2 + wy2);
	ret.set(2, 1, yz2 - wx2);
	ret.set(2, 2, ww - xx - yy + zz);
	ret.set(2, 3, 0.0f);

	ret.set(3, 0, 0.0f);
	ret.set(3, 1, 0.0f);
	ret.set(3, 2, 0.0f);
	ret.set(3, 3, 1.0f);
	return ret;
}

Vec4 Vec4::scaled(float s) const {
    return Vec4(x * s, y * s, z * s);
}

void Vec4::scale(float s) {
    x *= s;
    y *= s;
    z *= s;
}

Vec4 Vec4::normalized() const {
    float len = sqrt(lensq());

    // if length is zero, choice is arbirary
    if(fabs(len) < FLT_EPSILON) return Vec4(1, 0, 0);
    return scaled(1.0 / len);
}

void Vec4::normalize() {
    *this = this->normalized();
}

Vec4 Vec4::proj(const Vec4 &axis) const {
    Vec4 ret = axis;
    float numer = dot(axis);
    float denom = axis.dot(axis);
    if(fabs(denom) > FLT_EPSILON) {
        ret.scale(numer / denom);
    }
    return ret;
}

Vec4 Vec4::orth(const Vec4 &axis) const {
    Vec4 p = proj(axis);
    return *this - p;
}

Vec4 Vec4::cross(const Vec4 &o) const {
    return Vec4(y * o.z - z * o.y,
                z * o.x - x * o.z,
                x * o.y - y * o.x);
}

float Vec4::dot(const Vec4 &o) const {
    return (x * o.x) + (y * o.y) + (z * o.z) + (w * o.w);
}

float Vec4::lensq() const {
    return dot(*this);
}

float Vec4::length() const {
    return sqrt(lensq());
}


float Vec4::distsq(const Vec4 &o) const {
    return (x - o.x) * (x - o.x) +
           (y - o.y) * (y - o.y) +
           (z - o.z) * (z - o.z) +
           (w - o.w) * (w - o.w);
}

float Vec4::distance(const Vec4 &o) const {
    return sqrt(distsq(o));
}


void Vec4::print() {
    printf("%f %f %f %f\n", x, y, z, w);
}
