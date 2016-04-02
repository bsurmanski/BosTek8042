#include "mesh.hpp"

using GL::Mesh;
using GL::Vertex;
using GL::Face;

Mesh::Mesh() {
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
    nelems = 0;
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}

void Mesh::uploadVertexData(Vertex *verts, int nverts) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * nverts, 
                 verts, GL_STATIC_DRAW);
}

void Mesh::uploadFaceData(Face *faces, int nfaces) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Face) * nfaces, 
                 faces, GL_STATIC_DRAW);
    nelems = nfaces * 3;
}

void Mesh::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

#include <stdio.h>
void Mesh::draw() {
     glDrawElements(GL_TRIANGLES, nelems, GL_UNSIGNED_SHORT, NULL);
}
