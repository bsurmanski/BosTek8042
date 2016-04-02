#include "mdl.hpp"

#include <stdint.h>

using GL::Mesh;
using GL::Vertex;
using GL::Face;

struct MdlHeader {
    uint8_t magic[3];
    uint8_t version;
    uint32_t nverts;
    uint32_t nfaces;
    uint32_t nedges;
    uint8_t nbones;
    uint8_t name[15];
};

Mesh *loadMdl(Input *in) {
    MdlHeader header;
    in->read(&header, sizeof(MdlHeader), 1);

    if(header.magic[0] != 'M' || 
       header.magic[1] != 'D' || 
       header.magic[2] != 'L') {
        return NULL; // invalid MDL file. TODO: throw
    }

    uint32_t nedges = 0;
    if(header.version >= 5) {
        nedges = header.nedges;
    }

    Vertex *verts = new Vertex[header.nverts];
    Face *faces = new Face[header.nfaces];

    in->read(verts, sizeof(Vertex), header.nverts);
    in->read(faces, sizeof(Face), header.nfaces);

    Mesh *mesh = new Mesh;
    mesh->uploadVertexData(verts, header.nverts);
    mesh->uploadFaceData(faces, header.nfaces);
    
    delete[] verts;
    delete[] faces;

    return mesh;
}
