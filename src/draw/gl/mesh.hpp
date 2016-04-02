#ifndef _DRAW_GL_MESH_HPP
#define _DRAW_GL_MESH_HPP

#include <GL/glew.h>
#include <GL/gl.h>
#include <stdint.h>

namespace GL {

struct Vertex {
    float position[3];
    int16_t normal[3];
    uint16_t uv[2];
    uint8_t color[3];
    uint8_t material;
    uint8_t boneid[2];
    uint8_t boneweight[2];
    uint16_t incident_edge_id; // some incident edge of vertex
};

struct Face {
    uint16_t vertex_ids[3];
    uint16_t incident_edge_id;
};

struct Edge {
    uint16_t vertex_ids[2]; // first and second vert that forms an edge
    uint16_t face_ids[2]; // left and right face (when viewed from vert[0] to vert[1]
    uint16_t first_edges[2]; // edges cw and ccw from vertex[0]
    uint16_t second_edges[2]; // edges cw and ccw from vertex[1]
};

// Mesh representation in GPU memory
class Mesh {
    GLuint ibo;
    GLuint vbo;
    int nelems;

    public:
    Mesh();
    ~Mesh();
    void uploadVertexData(Vertex *verts, int nverts);
    void uploadFaceData(Face *faces, int nfaces);
    void bind();
    void draw();
};

};

#endif
