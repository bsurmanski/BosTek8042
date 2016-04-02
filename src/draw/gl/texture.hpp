#ifndef _DRAW_GL_TEXTURE_HPP
#define _DRAW_GL_TEXTURE_HPP

#include <GL/glew.h>
#include <GL/gl.h>

#include "draw/tga.hpp"
#include "draw/image.hpp"
#include "framebuffer.hpp"

namespace GL {

class Texture {
    protected:
    GLuint id;
    int w, h;

    Texture(int _w, int _h);
    Texture();

    public:
    virtual ~Texture();

    void setLinearFiltering();
    void setNearestFiltering();
    void setEdgeClamp();
    void setEdgeMirror();
    void setEdgeRepeat();
    void bind(int index=0);

    virtual bool isRGBA8();
    virtual bool isDepth();
    virtual bool isDepthStencil();

    friend class Framebuffer;
};

class RGBATexture : public Texture {
    public:
    RGBATexture(int w, int h);
    RGBATexture(const Image *img);
    static RGBATexture *FromTga(Input &img);
    virtual bool isRGBA8();
};

class DepthTexture : public Texture {
    public:
    DepthTexture(int w, int h);
    virtual bool isDepth();
};

class DepthStencilTexture : public Texture {
    public:
    DepthStencilTexture(int w, int h);
    virtual bool isDepthStencil();
};

}

#endif
