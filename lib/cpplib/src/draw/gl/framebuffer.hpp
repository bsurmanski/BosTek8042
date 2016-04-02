#ifndef _DRAW_GL_FRAMEBUFFER_HPP
#define _DRAW_GL_FRAMEBUFFER_HPP

#include "texture.hpp"


namespace GL {

class Texture;

class Framebuffer {
    GLuint id;
    int ntargets;

    public:
    Framebuffer();
    ~Framebuffer();
    void bind();
    void addTarget(Texture *t);
};

}

#endif
