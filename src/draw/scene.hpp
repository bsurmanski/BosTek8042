#ifndef _DRAW_SCENE_HPP
#define _DRAW_SCENE_HPP

#include "common/input.hpp"
#include "common/string.hpp"
#include "draw/gl/mesh.hpp"
#include "draw/gl/texture.hpp"
#include "draw/entity.hpp"

#include "geom/mat.hpp"

#include <map>
#include <vector>

class Scene {
    std::map<String, GL::Mesh*> meshes;
    std::map<String, GL::Texture*> textures;
    std::vector<Entity*> entities;

    public:
    void registerMesh(const String &k, GL::Mesh *m);
    void registerTexture(const String &k, GL::Texture *t);
    void loadMeshFromFile(const String &k, const String &filenm);
    void loadTextureFromFile(const String &k, const String &filenm);
    GL::Mesh *getMesh(const String &k);
    GL::Texture *getTexture(const String &k);
    Entity *addEntity(const String &k);
    Entity *addEntity(const String &mk, const String &tk);
};

void loadScn(Scene *s, Input *in);

#endif
