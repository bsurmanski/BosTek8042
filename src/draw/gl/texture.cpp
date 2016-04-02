#include "texture.hpp"

using GL::Texture;
using GL::RGBATexture;
using GL::DepthTexture;
using GL::DepthStencilTexture;

Texture::Texture(int _w, int _h) : w(_w), h(_h) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    // OpenGL picks the worst possible defaults for filtering. set up our own.
    setNearestFiltering();
    setEdgeRepeat();
}

Texture::Texture() : w(0), h(0) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    // OpenGL picks the worst possible defaults for filtering. set up our own.
    setNearestFiltering();
    setEdgeRepeat();
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::setLinearFiltering() {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::setNearestFiltering() {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::setEdgeClamp() {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture::setEdgeMirror() {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
}

void Texture::setEdgeRepeat() {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::bind(int index) {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, id);
}

bool Texture::isRGBA8() {
    return false;
}

bool Texture::isDepth() {
    return false;
}

bool Texture::isDepthStencil() {
    return false;
}

RGBATexture::RGBATexture(int w, int h) : Texture(w, h) {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 
        0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}

RGBATexture::RGBATexture(const Image *img) : Texture(img->getWidth(), img->getHeight()) {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 
        0, GL_RGBA, GL_UNSIGNED_BYTE, img->getPixelPtr());
}

RGBATexture *RGBATexture::FromTga(Input &input) {
    Image *img = loadTga(input);
    RGBATexture *tex = new RGBATexture(img);
    delete img;
    return tex;
}

bool RGBATexture::isRGBA8() {
    return true;
}

DepthTexture::DepthTexture(int w, int h) : Texture(w, h) {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 
        0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

bool DepthTexture::isDepth() {
    return true;
}

DepthStencilTexture::DepthStencilTexture(int w, int h) : Texture(w, h) {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 
        0, GL_DEPTH_STENCIL, GL_FLOAT, NULL);
}

bool DepthStencilTexture::isDepthStencil() {
    return true;
}
