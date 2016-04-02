#include "entity.hpp"

Entity::Entity(GL::Mesh *_mesh, GL::Texture *_texture) : 
    mesh(_mesh), 
    texture(_texture),
    scale(Vec4(1.0, 1.0, 1.0, 0.0)) {
}

Mat4 Entity::getModelMatrix() {
    Mat4 mat;
    mat.scale(scale.x, scale.y, scale.z);
    mat = mat * rotation;
    mat.translate(position);
    return mat;
}
