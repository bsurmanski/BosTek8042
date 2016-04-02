#include "scene.hpp"

#include "common/exception.hpp"
#include "common/file.hpp"
#include "draw/gl/mdl.hpp"
#include "draw/tga.hpp"

#include <stdint.h>

void Scene::registerMesh(const String &k, GL::Mesh *m) {
    meshes[k] = m;
}

void Scene::registerTexture(const String &k, GL::Texture *t) {
    textures[k] = t;
}

void Scene::loadMeshFromFile(const String &k, const String &filenm) {
    File f(filenm);
    GL::Mesh *m = loadMdl(&f);
    meshes[k] = m;
}

void Scene::loadTextureFromFile(const String &k, const String &filenm) {
    File f(filenm);
    GL::Texture *t = GL::RGBATexture::FromTga(f);
    textures[k] = t;
}

GL::Mesh *Scene::getMesh(const String &k) {
    return meshes.at(k);
}

GL::Texture *Scene::getTexture(const String &k) {
    return textures.at(k);
}

Entity *Scene::addEntity(const String &k) {
    return addEntity(k, k);
}

Entity *Scene::addEntity(const String &mk, const String &tk) {
    GL::Mesh *m = meshes.at(mk);
    GL::Texture *t = textures.at(tk);
    Entity *e = new Entity(m, t);
    entities.push_back(e);
    return e;
}

struct SceneHeader {
    char magic[3];
    uint8_t version;
    uint16_t nentities;
    char padding[10];
    char name[16];
};

struct SceneEntity {
    uint16_t pid;
    char padding[2];
    float position[3];
    float scale[3];
    float rotation[4];
    char padding2[4];
    char name[16];
};

void loadScn(Scene *s, Input *in) {
    SceneHeader header;
    in->read(&header, sizeof(SceneHeader), 1);
    if(header.magic[0] != 'S' ||
       header.magic[1] != 'C' ||
       header.magic[2] != 'N') {
        throw Exception("invalid scene file");
    }

    for(int i = 0; i < header.nentities; i++) {
        SceneEntity ent;
        in->read(&ent, sizeof(SceneEntity), 1);
        String name(header.name);
        Entity *entity = s->addEntity(name);
        entity->position = Vec4(ent.position[0], ent.position[1], ent.position[2], 1.0f);
        entity->scale = Vec4(ent.scale[0], ent.scale[1], ent.scale[2]);
        entity->rotation = Vec4(ent.rotation[0],
                                 ent.rotation[1],
                                 ent.rotation[2],
                                 ent.rotation[3]).qToMat();
    }
}
