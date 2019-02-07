// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL-3.0

#define sphereDec

namespace sphere
{
    // public functions, to be called from outside
    const std::vector<GLfloat>& GetVerts();
    const std::vector<GLfloat>& GetVertsNorms();
    const std::vector<GLuint>& GetInds();
    void build(GLuint);
    void release();
    GLuint GetNInds();
}


