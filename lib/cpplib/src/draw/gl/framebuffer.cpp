#include "framebuffer.hpp"

using GL::Framebuffer;

static GLuint TARGETS[] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6,
    GL_COLOR_ATTACHMENT7,
    GL_COLOR_ATTACHMENT8,
    GL_COLOR_ATTACHMENT9,
};

Framebuffer::Framebuffer() : ntargets(0) {
    glGenFramebuffers(1, &id);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &id);
}

void Framebuffer::bind() {
   glBindFramebuffer(GL_FRAMEBUFFER, id);
   glDrawBuffers(ntargets, TARGETS);
}

void Framebuffer::addTarget(Texture *t) {
   glBindFramebuffer(GL_FRAMEBUFFER, id);

	if(t->isRGBA8()) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + ntargets,
			GL_TEXTURE_2D, t->id, 0);
		ntargets++;
	} else if(t->isDepth()) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, t->id, 0);
	} else if(t->isDepthStencil()) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
			GL_TEXTURE_2D, t->id, 0);
	}
}
