#ifndef _DRAW_GL_PROGRAM
#define _DRAW_GL_PROGRAM

#include "cpplib/common/input.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

namespace GL {

class DrawDevice;

class Program {
    protected:
    GLuint program;

    public:
    Program();
    virtual ~Program();
    virtual void bind();
    friend class DrawDevice;
};

class StdProgram : public Program {
    GLuint vso;
    GLuint fso;

    public:
    StdProgram(Input *vs, Input *fs);
    bool hasErrors();
};

}

#endif
