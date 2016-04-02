#ifndef _ENTITY_HPP
#define _ENTITY_HPP

#include "cpplib/geom/vec.hpp"
#include "cpplib/geom/mat.hpp"
#include "cpplib/draw/gl/mesh.hpp"
#include "cpplib/draw/gl/texture.hpp"

class Entity {
    public:
    GL::Mesh *mesh;
    GL::Texture *texture;

    Vec4 position;
    Vec4 scale;
    Mat4 rotation;


    Entity(GL::Mesh *mesh, GL::Texture *texture);
    Mat4 getModelMatrix();
};

#endif
