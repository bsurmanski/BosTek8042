#include "mat.hpp"

#include <stdio.h>
#include <math.h>

Mat4::Mat4() {
    v[0] = 1.0f;
    v[1] = 0.0f;
    v[2] = 0.0f;
    v[3] = 0.0f;

    v[4] = 0.0f;
    v[5] = 1.0f;
    v[6] = 0.0f;
    v[7] = 0.0f;

    v[8] = 0.0f;
    v[9] = 0.0f;
    v[10] = 1.0f;
    v[11] = 0.0f;

    v[12] = 0.0f;
    v[13] = 0.0f;
    v[14] = 0.0f;
    v[15] = 1.0f;
}

float *Mat4::ptr() {
    return v;
}

float &Mat4::operator[](int i) {
    return v[i];
}

float Mat4::operator[](int i) const {
    return v[i];
}

Mat4 Mat4::operator*(const Mat4 &o) {
    return mul(o);
}

Vec4 Mat4::operator*(const Vec4 &o) {
    return mul(o);
}

float &Mat4::get(int x, int y) {
    return v[y*4+x];
}

float Mat4::get(int x, int y) const {
    return v[y*4+x];
}

void Mat4::set(int x, int y, float val) {
    v[x+y*4] = val;
}

Mat4 Mat4::mul(const Mat4 &o) const {
    Mat4 ret;

    for(int j = 0; j < 4; j++)
    for(int i = 0; i < 4; i++) {
        ret[j*4+i] = 0.0f;
        for(int k = 0; k < 4; k++) {
            ret[j*4+i] += v[j*4+k] * o[k*4+i];
        }
    }

    return ret;
}

Vec4 Mat4::mul(const Vec4 &o) const {
    Vec4 ret;

    for(int j = 0; j < 4; j++) {
        ret[j] = 0.0f;
        for(int i = 0; i < 4; i++) {
            ret[j] += get(i, j) * o.get(i);
        }
    }

    return ret;
}

Mat4 Mat4::translated(const Vec4 &dv) const {
    Mat4 m;
    m.set(3, 0, dv[0]);
    m.set(3, 1, dv[1]);
    m.set(3, 2, dv[2]);
    return m.mul(*this);
}

Mat4 Mat4::rotated(float angle, const Vec4 &axis) const {
    Mat4 m;
    Vec4 r = axis.normalized();
    float c, s, t;
    s = sinf(angle);
    c = cosf(angle);
    t = 1.0f - c;

    m[0] = t * r[0] * r[0] + c;
    m[1] = t * r[0] * r[1] - s * r[2];
    m[2] = t * r[0] * r[2] + s * r[1];
    m[3] = 0.0f;

    m[4] = t * r[0] * r[1] + s * r[2];
    m[5] = t * r[1] * r[1] + c;
    m[6] = t * r[1] * r[2] - s * r[0];
    m[7] = 0.0f;

    m[8]  = t * r[0] * r[2] - s * r[1];
    m[9]  = t * r[1] * r[2] + s * r[0];
    m[10] = t * r[2] * r[2] + c;
    m[11] = 0.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;

    return m.mul(*this);
}

Mat4 Mat4::scaled(float x, float y, float z) const {
    Mat4 m;
    m.v[0] = x;
    m.v[5] = y;
    m.v[10] = z;
    return m.mul(*this);
}

Mat4 Mat4::transpose() const {
    Mat4 ret;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            ret.set(i, j, get(j, i));
        }
    }
    return ret;
}

void Mat4::translate(const Vec4 &dv) {
    *this = translated(dv);
}

void Mat4::rotate(float angle, const Vec4 &axis) {
    Mat4 m  = rotated(angle, axis);
    for(int i = 0; i < 16; i++) {
        v[i] = m[i];
    }
}

void Mat4::scale(float x, float y, float z) {
    *this = scaled(x, y, z);
}

void Mat4::setXVector(Vec4 v) {
    set(0, 0, v[0]);
    set(0, 1, v[1]);
    set(0, 2, v[2]);
    set(0, 3, v[3]);
}

void Mat4::setYVector(Vec4 v) {
    set(1, 0, v[0]);
    set(1, 1, v[1]);
    set(1, 2, v[2]);
    set(1, 3, v[3]);
}

void Mat4::setZVector(Vec4 v) {
    set(2, 0, v[0]);
    set(2, 1, v[1]);
    set(2, 2, v[2]);
    set(2, 3, v[3]);
}

void Mat4::setWVector(Vec4 v) {
    set(3, 0, v[0]);
    set(3, 1, v[1]);
    set(3, 2, v[2]);
    set(3, 3, v[3]);
}

void Mat4::setXRow(Vec4 v) {
    set(0, 0, v[0]);
    set(1, 0, v[1]);
    set(2, 0, v[2]);
    set(3, 0, v[3]);
}

void Mat4::setYRow(Vec4 v) {
    set(0, 1, v[0]);
    set(1, 1, v[1]);
    set(2, 1, v[2]);
    set(3, 1, v[3]);
}

void Mat4::setZRow(Vec4 v) {
    set(0, 2, v[0]);
    set(1, 2, v[1]);
    set(2, 2, v[2]);
    set(3, 2, v[3]);
}

void Mat4::setWRow(Vec4 v) {
    set(0, 3, v[0]);
    set(1, 3, v[1]);
    set(2, 3, v[2]);
    set(3, 3, v[3]);
}

void Mat4::zero() {
    for(int i = 0; i < 16; i++) {
        v[i] = 0.0f;
    }
}

void Mat4::print() {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            printf("%f ", get(j, i));
        }
        printf("\n");
    }
}

Mat4 Mat4::createFrustumMatrix(float l, float r, float b, float t, float n, float f) {
    Mat4 m;

    m[0] = 2.0f * n / (r - l);
    m[1] = 0.0f;
    m[2] = (r + l) / (r - l);
    m[3] = 0.0f;

    m[4] = 0.0f;
    m[5] = (2.0f * n) / (t - b);
    m[6] = (t + b) / (t - b);
    m[7] = 0.0f;

    m[8] = 0.0f;
    m[9] = 0.0f;
    m[10] = -(f + n) / (f - n);
    m[11] = -(2.0f * f * n) / (f - n);

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = -1.0f;
    m[15] = 0.0f;

    return m;

}
