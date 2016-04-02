#ifndef _GEOM_VEC_HPP
#define _GEOM_VEC_HPP

class Mat4;
class Vec4 {
    public:
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };

        struct {
            float r;
            float g;
            float b;
            float a;
        };

        float v[4];
    };

    Vec4();
    Vec4(float x, float y, float z, float w=0);
    static Vec4 createQuat(float angle, const Vec4 &axis);
    Vec4 &operator+=(const Vec4 &o);
    Vec4 &operator-=(const Vec4 &o);
    Vec4 operator+(const Vec4 &o) const;
    Vec4 operator-(const Vec4 &o) const;
    float &operator[](int i);
    float operator[](int i) const;
    Vec4 operator*(const Mat4 &o) const;
    Vec4 mul(const Mat4 &o) const;
    Vec4 qMul(const Vec4 &o) const;
    Vec4 qRotate(const Vec4 &p) const;
    Vec4 qConj() const;
    Mat4 qToMat() const;
    float &get(int i);
    float get(int i) const;
    void set(int i, float v);
    Vec4 scaled(float s) const;
    void scale(float s);
    Vec4 normalized() const;
    void normalize();
    Vec4 proj(const Vec4 &axis) const;
    Vec4 orth(const Vec4 &axis) const;
    Vec4 cross(const Vec4 &o) const;
    float dot(const Vec4 &o) const;
    float length() const;
    float lensq() const;
    float distsq(const Vec4 &o) const;
    float distance(const Vec4 &o) const;
    void print();
};

#endif
