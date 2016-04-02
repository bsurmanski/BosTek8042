#ifndef _ENTITY_HPP
#define _ENTITY_HPP

#include "geom/vec.hpp"
#include "geom/mat.hpp"
#include "draw/gl/mesh.hpp"
#include "draw/gl/texture.hpp"

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
