#include "camera.hpp"

Camera::Camera() {
    frustum = Mat4::createFrustumMatrix(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10000.0f);
    position = Vec4(0, 0, 0, 1);
    fwd = Vec4(0, 0, 1, 0);
    up = Vec4(0, 1, 0, 0);
}

void Camera::setPosition(const Vec4 &_pos) {
    position = _pos;
}

void Camera::setUpVector(const Vec4 &_up) {
    up = _up;
}

void Camera::setForwardVector(const Vec4 &_fwd) {
    fwd = _fwd;
}

void Camera::move(const Vec4 &offset) {
    position += offset;
}

void Camera::rotate(float x, float y, float z) {
}

void Camera::setFrustum(const Mat4 &f) {
    frustum = f;
}

Mat4 Camera::getTransform() {
    Vec4 mfwd = fwd.normalized();
    Vec4 mup = up.orth(mfwd).normalized();
    Vec4 mright = mfwd.cross(mup).normalized();
    Mat4 t;
    t.setXVector(mright);
    t.setYVector(mup);
    t.setZVector(mfwd);
    t.translate(position);
    return frustum * t;
}
