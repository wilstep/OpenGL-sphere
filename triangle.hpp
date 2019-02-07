// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL-3.0

#ifndef triangleDec
#define triangleDec


class triangle
{
public:
    void set(GLuint t0, GLuint t1, GLuint t2, GLuint v0, GLuint v1, GLuint v2);
    void push_stack();
    GLuint top_stack(GLuint i) { return lifo.top().at(i); }
    void pop_stack() { lifo.pop(); }
    void putIndex(GLuint i, GLuint xi) { index.at(i) = xi; }
    GLuint getIndex(GLuint i) { return index.at(i); }
    auto& getIndex() { return index; }
    void putNeigh_tri(GLuint i, GLuint xi) { neigh_tri.at(i) = xi; }
    GLuint getNeigh_tri(GLuint i) { return neigh_tri.at(i); }
private:
    // private data
    std::stack<std::array<GLuint, 3> > lifo;
    // 3 indices to vetercies which form triangle
    std::array<GLuint, 3> index;  
    // 3 indices of neighbouring triangles
    std::array<GLuint, 3> neigh_tri;   
};

#endif


