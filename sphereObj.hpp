// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL-3.0

#ifndef sphereObjDec
#define sphereObjDec

class triangle;

class sphereObj
{
public:
    sphereObj(GLuint n);
    const std::vector<GLuint>& GetInds();
    const std::vector<GLfloat>& GetVerts(){ return verts; }
    GLuint GetNInds(){ return 3 * nTri; }
private:
    void octahedron(); 
    void triDivide();
    void newNeighbours(GLuint itr, GLuint i);
    GLuint getTrig(GLuint itr, GLuint pivot);
    void setIndex(triangle &tr);
    void newVertex(GLuint j, GLuint k);
    
    // private data
    std::vector<GLfloat> verts;
    std::vector<triangle> trigs;
    std::vector<GLuint> inds;
    const GLuint order, nVerts, nTri;
    GLuint triCnt, triCntOld, vertCnt, vertCntOld;
};

#endif

#ifndef triangleDec
#include "triangle.hpp"
#endif
