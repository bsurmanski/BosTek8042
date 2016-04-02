#ifndef _DRAW_CAMERA_HPP
#define _DRAW_CAMERA_HPP

#include "cpplib/geom/vec.hpp"
#include "cpplib/geom/mat.hpp"

class Camera {
    Vec4 position;
    Vec4 fwd;
    Vec4 up;
    Mat4 frustum;

    public:
    Camera();
    void setPosition(const Vec4 &pos);
    void setUpVector(const Vec4 &up);
    void setForwardVector(const Vec4 &fwd);
    void move(const Vec4 &offset);
    void rotate(float x, float y, float z);
    void setFrustum(const Mat4 &f);
    Mat4 getTransform();
};

#endif
