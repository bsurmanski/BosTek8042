#include "program.hpp"

#include <stdio.h>

using GL::Program;
using GL::StdProgram;

static bool shaderHasCompileErrors(GLuint shader) {
	int err;
	char buf[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &err);
	if(err != GL_TRUE) {
		glGetShaderInfoLog(shader, 512, NULL, buf);
		printf("VS ERR: %s\n", buf);
        return true;
	}
    return false;
}

static bool programHasLinkErrors(GLuint program) {
	int err;
	char buf[512];
	glGetProgramiv(program, GL_LINK_STATUS, &err);
	if(err != GL_TRUE) {
		glGetProgramInfoLog(program, 512, NULL, buf);
		printf("GLProgram Link Error: %s\n", buf);
        return true;
	}
    return false;
}

Program::Program() {
    program  = glCreateProgram();
}

Program::~Program() {
    glDeleteProgram(program);
}

void Program::bind() {
    glUseProgram(program);
}

StdProgram::StdProgram(Input *vs, Input *fs) {
    vso = glCreateShader(GL_VERTEX_SHADER);
    fso = glCreateShader(GL_FRAGMENT_SHADER);

    char *vs_src;
    char *fs_src;

    vs_src = new char[vs->getSize()+1];
    fs_src = new char[fs->getSize()+1];

    vs->read(vs_src, 1, vs->getSize());
    fs->read(fs_src, 1, fs->getSize());
    vs_src[vs->getSize()] = '\0';
    fs_src[fs->getSize()] = '\0';

    glShaderSource(vso, 1, (const GLchar**) &vs_src, (GLint*) NULL);
    glShaderSource(fso, 1, (const GLchar**) &fs_src, (GLint*) NULL);

    delete[] vs_src;
    delete[] fs_src;

    glCompileShader(vso);
    glCompileShader(fso);

    if(shaderHasCompileErrors(vso) || shaderHasCompileErrors(fso)) {
        return; //TODO throw
    }

	glAttachShader(program, vso);
	glAttachShader(program, fso);
    glLinkProgram(program);

    if(programHasLinkErrors(program)) {
        return; //TODO throw
    }
}
