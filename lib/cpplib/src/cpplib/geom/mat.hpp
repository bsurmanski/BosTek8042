#ifndef _GEOM_MAT_HPP
#define _GEOM_MAT_HPP

#include "vec.hpp"

class Mat4 {
    float v[16];

    public:
    Mat4();

    float *ptr();

    float &operator[](int);
    float operator[](int) const;
    Mat4 operator*(const Mat4 &o);
    Vec4 operator*(const Vec4 &o);

    float &get(int x, int y);
    float get(int x, int y) const;
    void set(int x, int y, float val);
    Mat4 mul(const Mat4 &o) const;
    Vec4 mul(const Vec4 &o) const;

    Mat4 translated(const Vec4 &dv) const;
    Mat4 rotated(float angle, const Vec4 &axis) const;
    Mat4 scaled(float x, float y, float z) const;

    Mat4 transpose() const;

    void translate(const Vec4 &dv);
    void rotate(float angle, const Vec4 &axis);
    void scale(float x, float y, float z);

    void setXVector(Vec4 v);
    void setYVector(Vec4 v);
    void setZVector(Vec4 v);
    void setWVector(Vec4 v);

    void setXRow(Vec4 v);
    void setYRow(Vec4 v);
    void setZRow(Vec4 v);
    void setWRow(Vec4 v);

    void zero();
    void print();

    static Mat4 createFrustumMatrix(float l, float r, float b, float t, float n, float f);
};

#endif
